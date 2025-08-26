/* 
 * File:   TWSERMDataPool.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/01/06 15:38
 */

#include "TWSERMDataPool.h"
#include "iniFile.h"

namespace twserm
{
CRiskManagerDataPool* gRMDataPoolPtr = 0;

//---------------------------- CRiskManagerDataPool ----------------------------
nsOrderMessageDefine::MarketEnum CRiskManagerDataPool::FMarket     = nsOrderMessageDefine::mTSE;
UFC::AnsiString                  CRiskManagerDataPool::FMarketName = "TSE";

UFC::AnsiString CRiskManagerDataPool::FRiskManagerID         = "RMTWSE";
UFC::AnsiString CRiskManagerDataPool::FConfigureFilePathName = "../cfg/TWSERiskManager.cfg";

UFC::AnsiString CRiskManagerDataPool::FLogFilePath         = "log";
UFC::AnsiString CRiskManagerDataPool::FFormatFilePath      = "../cfg";
UFC::AnsiString CRiskManagerDataPool::FTSEStockDataPath    = "../ftTSE";
UFC::AnsiString CRiskManagerDataPool::FOTCStockDataPath    = "../ftOTC";

UFC::AnsiString CRiskManagerDataPool::FTSET30FormatFileName = "TSE_T33.ini";
UFC::AnsiString CRiskManagerDataPool::FTSET32FormatFileName = "TSE_T32.ini";
UFC::AnsiString CRiskManagerDataPool::FTSET33FormatFileName = "TSE_T33.ini";
UFC::AnsiString CRiskManagerDataPool::FTSEO40FormatFileName = "TSE_O40.ini";
UFC::AnsiString CRiskManagerDataPool::FTSEO60FormatFileName = "TSE_O60";
UFC::AnsiString CRiskManagerDataPool::FTSET30FileName = "T30";
UFC::AnsiString CRiskManagerDataPool::FTSET32FileName = "T32";
UFC::AnsiString CRiskManagerDataPool::FTSET33FileName = "T33";
UFC::AnsiString CRiskManagerDataPool::FTSEO40FileName = "O40";
UFC::AnsiString CRiskManagerDataPool::FTSEO60FileName = "O60";

UFC::AnsiString CRiskManagerDataPool::FOTCT30FormatFileName = "OTC_T30.ini";
UFC::AnsiString CRiskManagerDataPool::FOTCT32FormatFileName = "OTC_T32.ini";
UFC::AnsiString CRiskManagerDataPool::FOTCT33FormatFileName = "OTC_T33.ini";
UFC::AnsiString CRiskManagerDataPool::FOTCO40FormatFileName = "OTC_O40.ini";
UFC::AnsiString CRiskManagerDataPool::FOTCO60FormatFileName = "OTC_O60.ini";
UFC::AnsiString CRiskManagerDataPool::FOTCT30FileName = "T30";
UFC::AnsiString CRiskManagerDataPool::FOTCT32FileName = "T32";
UFC::AnsiString CRiskManagerDataPool::FOTCT33FileName = "T33";
UFC::AnsiString CRiskManagerDataPool::FOTCO40FileName = "O40";
UFC::AnsiString CRiskManagerDataPool::FOTCO60FileName = "O60";

UFC::AnsiString CRiskManagerDataPool::FTSET30FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FTSET32FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FTSET33FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FTSEO40FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FTSEO60FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FTSET30FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FTSET32FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FTSET33FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FTSEO40FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FTSEO60FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT30FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT32FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT33FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FOTCO40FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FOTCO60FormatFilePathName = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT30FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT32FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FOTCT33FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FOTCO40FilePathName       = "";
UFC::AnsiString CRiskManagerDataPool::FOTCO60FilePathName       = "";

bool CRiskManagerDataPool::FIsTSET30Ready = false;
bool CRiskManagerDataPool::FIsTSET32Ready = false;
bool CRiskManagerDataPool::FIsTSET33Ready = false;
bool CRiskManagerDataPool::FIsTSEO40Ready = false;
bool CRiskManagerDataPool::FIsTSEO60Ready = false;
bool CRiskManagerDataPool::FIsOTCT30Ready = false;
bool CRiskManagerDataPool::FIsOTCT32Ready = false;
bool CRiskManagerDataPool::FIsOTCT33Ready = false;
bool CRiskManagerDataPool::FIsOTCO40Ready = false;
bool CRiskManagerDataPool::FIsOTCO60Ready = false;

UFC::BufferedLog* CRiskManagerDataPool::FLogPtr = 0;
bool CRiskManagerDataPool::FIsRecoverMode = false;

SecurityInfoMap CRiskManagerDataPool::FStockMap;

//------------------------------------------------------------------------------
UFC::AnsiString CRiskManagerDataPool::ComposeFilePathName(const UFC::AnsiString& FilePath, const UFC::AnsiString& FileName)
{
    if (FileName.Length() <= 0) return "";
    
    UFC::PStringBuffer pathNameBuffer;
    int pathLength = FilePath.Length();
    if (pathLength > 0)
    {
        pathNameBuffer.Append(FilePath);
        if (FilePath[pathLength - 1] != '/') pathNameBuffer.Append('/');
    }
    pathNameBuffer.Append(FileName);
    return pathNameBuffer.ToString();
}  //CRiskManagerDataPool::ComposeFilePathName()
//------------------------------------------------------------------------------
UFC::BufferedLog* CRiskManagerDataPool::CreateLogObject()
{
    if (FLogPtr != 0)
    {
        FLogPtr->Flush();
        delete FLogPtr;
    }
    
    UFC::AnsiString logSectionName;
    logSectionName.Printf( "%s log", FMarketName.c_str() );
    UFC::AnsiString logFileNamePrefix;
    logFileNamePrefix.Printf( "rm%s", FMarketName.c_str() );
    if (FIsRecoverMode)
        FLogPtr = new UFC::BufferedLog(logSectionName, FLogFilePath, logFileNamePrefix, "log", 1048576, TRUE);  //Append
    else
        FLogPtr = new UFC::BufferedLog(logSectionName, FLogFilePath, logFileNamePrefix, "log", 1048576, FALSE);  //Create
    UFC::BufferedLog::Printf(" CRiskManagerDataPool::%s() Create %s %s RiskManager Log file.", __func__, FRiskManagerID.c_str(), FMarketName.c_str());    
    return FLogPtr;
}  //CRiskManagerDataPool::CreateLogObject()
//------------------------------------------------------------------------------
void CRiskManagerDataPool::DeleteLogObject()
{
    if (FLogPtr != 0)
    {
        FLogPtr->Flush();
        delete FLogPtr;
        FLogPtr = 0;
    }
}  //CRiskManagerDataPool::DeleteLogObject()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadTSET30File()
{
    FIsTSET30Ready = FStockMap.LoadT30(nsOrderMessageDefine::mTSE, FTSET30FormatFilePathName, FTSET30FilePathName, FLogPtr);
    return FIsTSET30Ready;
}  //CRiskManagerDataPool::LoadTSET30File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadTSET32File()
{
    FIsTSET32Ready = FStockMap.LoadT32(nsOrderMessageDefine::mTSE, FTSET32FormatFilePathName, FTSET32FilePathName, FLogPtr);
    return FIsTSET32Ready;
}  //CRiskManagerDataPool::LoadTSET32File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadTSET33File()
{
    FIsTSET33Ready = FStockMap.LoadT33(nsOrderMessageDefine::mTSE, FTSET33FormatFilePathName, FTSET33FilePathName, FLogPtr);    
    return FIsTSET33Ready;
}  //CRiskManagerDataPool::LoadTSET33File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadTSEO40File()
{
    FIsTSEO40Ready = FStockMap.LoadO40(nsOrderMessageDefine::mTSE, FTSEO40FormatFilePathName, FTSEO40FilePathName, FLogPtr);    
    return FIsTSEO40Ready;
}  //CRiskManagerDataPool::LoadTSEO40File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadTSEO60File()
{
    FIsTSEO60Ready = FStockMap.LoadO60(nsOrderMessageDefine::mTSE, FTSEO60FormatFilePathName, FTSEO60FilePathName, FLogPtr);    
    return FIsTSEO60Ready;
}  //CRiskManagerDataPool::LoadTSEO60File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadOTCT30File()
{
    FIsOTCT30Ready = FStockMap.LoadT30(nsOrderMessageDefine::mOTC, FOTCT30FormatFilePathName, FOTCT30FilePathName, FLogPtr);
    return FIsOTCT30Ready;
}  //CRiskManagerDataPool::LoadOTCT30File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadOTCT32File()
{
    FIsOTCT32Ready = FStockMap.LoadT32(nsOrderMessageDefine::mOTC, FOTCT32FormatFilePathName, FOTCT32FilePathName, FLogPtr);
    return FIsOTCT32Ready;
}  //CRiskManagerDataPool::LoadOTCT32File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadOTCT33File()
{
    FIsOTCT33Ready = FStockMap.LoadT33(nsOrderMessageDefine::mOTC, FOTCT33FormatFilePathName, FOTCT33FilePathName, FLogPtr);    
    return FIsOTCT33Ready;
}  //CRiskManagerDataPool::LoadOTCT33File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadOTCO40File()
{
    FIsOTCO40Ready = FStockMap.LoadO40(nsOrderMessageDefine::mTSE, FOTCO40FormatFilePathName, FOTCO40FilePathName, FLogPtr);    
    return FIsOTCO40Ready;
}  //CRiskManagerDataPool::LoadOTCO40File()
//------------------------------------------------------------------------------
bool CRiskManagerDataPool::LoadOTCO60File()
{
    FIsOTCO60Ready = FStockMap.LoadO60(nsOrderMessageDefine::mTSE, FOTCO60FormatFilePathName, FOTCO60FilePathName, FLogPtr);    
    return FIsOTCO60Ready;
}  //CRiskManagerDataPool::LoadOTCO60File()
//------------------------------------------------------------------------------
void CRiskManagerDataPool::LoadStockDataFile()
{
    if (!FIsTSET30Ready && (UFC::FileExists(FTSET30FormatFilePathName) == TRUE) && (UFC::FileExists(FTSET30FilePathName) == TRUE))
        LoadTSET30File();
    if (FIsTSET30Ready)
    {
        if (!FIsTSET32Ready && (UFC::FileExists(FTSET32FormatFilePathName) == TRUE) && (UFC::FileExists(FTSET32FilePathName) == TRUE))
            LoadTSET32File();
        
        if (!FIsTSET33Ready && (UFC::FileExists(FTSET33FormatFilePathName) == TRUE) && (UFC::FileExists(FTSET33FilePathName) == TRUE))
            LoadTSET33File();
        
        if (!FIsTSEO40Ready && (UFC::FileExists(FTSEO40FormatFilePathName) == TRUE) && (UFC::FileExists(FTSEO40FilePathName) == TRUE))
            LoadTSEO40File();
        
        if (!FIsTSEO60Ready && (UFC::FileExists(FTSEO60FormatFilePathName) == TRUE) && (UFC::FileExists(FTSEO60FilePathName) == TRUE))
            LoadTSEO60File();
    }
    
    if (!FIsOTCT30Ready && (UFC::FileExists(FOTCT30FormatFilePathName) == TRUE) && (UFC::FileExists(FOTCT30FilePathName) == TRUE))
        LoadOTCT30File();
    if (FIsOTCT30Ready)
    {
        if (!FIsOTCT32Ready && (UFC::FileExists(FOTCT32FormatFilePathName) == TRUE) && (UFC::FileExists(FOTCT32FilePathName) == TRUE))
            LoadOTCT32File();
        
        if (!FIsOTCT33Ready && (UFC::FileExists(FOTCT33FormatFilePathName) == TRUE) && (UFC::FileExists(FOTCT33FilePathName) == TRUE))
            LoadOTCT33File();
        
        if (!FIsOTCO40Ready && (UFC::FileExists(FOTCO40FormatFilePathName) == TRUE) && (UFC::FileExists(FOTCO40FilePathName) == TRUE))
            LoadOTCO40File();
        
        if (!FIsOTCO60Ready && (UFC::FileExists(FOTCO60FormatFilePathName) == TRUE) && (UFC::FileExists(FOTCO60FilePathName) == TRUE))
            LoadOTCO60File();
    }
}  //CRiskManagerDataPool::LoadStockDataFile()
//------------------------------------------------------------------------------
SecurityInfo* CRiskManagerDataPool::FindStock(const UFC::AnsiString& StockNo)
{
    SecurityInfo* stockPtr = FStockMap.GetSecurityInfoByStockNo(StockNo);
    return stockPtr;
}  //CRiskManagerDataPool::FindStock()
//------------------------------------------------------------------------------
void CRiskManagerDataPool::DumpStockData()
{
    FStockMap.DumpStocks(FLogPtr);
}  //CRiskManagerDataPool::DumpStockData()

//------------------------------------------------------------------------------
CRiskManagerDataPool::CRiskManagerDataPool()
{
}  //CRiskManagerDataPool::CRiskManagerDataPool()
//------------------------------------------------------------------------------
CRiskManagerDataPool::~CRiskManagerDataPool()
{
}  //CRiskManagerDataPool::~CRiskManagerDataPool()
//------------------------------------------------------------------------------
void CRiskManagerDataPool::LoadSetting()
{
    UFC::UiniFile *configFilePtr = NULL;
    if (UFC::FileExists(FConfigureFilePathName) == TRUE)
    {
        try
        {
            configFilePtr = new UFC::UiniFile(FConfigureFilePathName);
        }
        catch(UFC::FileException& fe)
        {
            FLogPtr->fprintf(" CRiskManagerDataPool::%s() Load Configure File[%s] failed. %s", __func__, FConfigureFilePathName.c_str(), fe.what());
            configFilePtr = 0;
        }
    }
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() Load Configure File[%s] not Exists", __func__, FConfigureFilePathName.c_str());
    
    if (configFilePtr == NULL)
    {
        FLogPtr->Flush();
        return;
    }

    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "StockFormatFilePath";
    UFC::AnsiString fieldValue = "";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FFormatFilePath = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FFormatFilePath.c_str());

    fieldName = "TSEStockT30FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET30FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET30FormatFileName.c_str());
    
    fieldName = "TSEStockT32FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET32FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET32FormatFileName.c_str());
    
    fieldName = "TSEStockT33FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET33FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET33FormatFileName.c_str());
    
    fieldName = "TSEStockO40FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSEO40FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSEO40FormatFileName.c_str());
    
    fieldName = "TSEStockO60FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSEO60FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSEO60FormatFileName.c_str());
    
    fieldName = "OTCStockT30FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT30FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT30FormatFileName.c_str());
    
    fieldName = "OTCStockT32FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT32FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT32FormatFileName.c_str());
    
    fieldName = "OTCStockT33FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT33FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT33FormatFileName.c_str());
    
    fieldName = "OTCStockO40FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCO40FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCO40FormatFileName.c_str());
    
    fieldName = "OTCStockO60FormatFileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCO60FormatFileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCO60FormatFileName.c_str());
    
    fieldName = "TSEStockFilePath";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSEStockDataPath = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSEStockDataPath.c_str());
    
    fieldName = "TSEStockT30FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET30FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET30FileName.c_str());
    
    fieldName = "TSEStockT32FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET32FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET32FileName.c_str());
    
    fieldName = "TSEStockT33FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSET33FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSET33FileName.c_str());
    
    fieldName = "TSEStockO40FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSEO40FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSEO40FileName.c_str());
    
    fieldName = "TSEStockO60FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FTSEO60FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FTSEO60FileName.c_str());
    
    fieldName = "OTCStockFilePath";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCStockDataPath = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCStockDataPath.c_str());
    
    fieldName = "OTCStockT30FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT30FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT30FileName.c_str());
    
    fieldName = "OTCStockT32FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT32FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT32FileName.c_str());
    
    fieldName = "OTCStockT33FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCT33FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCT33FileName.c_str());
    
    fieldName = "OTCStockO40FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCO40FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCO40FileName.c_str());
    
    fieldName = "OTCStockO60FileName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        FOTCO60FileName = fieldValue;
    else
        FLogPtr->fprintf(" CRiskManagerDataPool::%s() [%s-%s] not Setting, use Default Value:[%s]", __func__, sectionName.c_str(), fieldName.c_str(), FOTCO60FileName.c_str());
    
    delete configFilePtr;
    
    FTSET30FormatFilePathName = ComposeFilePathName(FFormatFilePath, FTSET30FormatFileName);
    FTSET32FormatFilePathName = ComposeFilePathName(FFormatFilePath, FTSET32FormatFileName);
    FTSET33FormatFilePathName = ComposeFilePathName(FFormatFilePath, FTSET33FormatFileName);
    FTSEO40FormatFilePathName = ComposeFilePathName(FFormatFilePath, FTSEO40FormatFileName);
    FTSEO60FormatFilePathName = ComposeFilePathName(FFormatFilePath, FTSEO60FormatFileName);
    FTSET30FilePathName       = ComposeFilePathName(FTSEStockDataPath, FTSET30FileName);
    FTSET32FilePathName       = ComposeFilePathName(FTSEStockDataPath, FTSET32FileName);
    FTSET33FilePathName       = ComposeFilePathName(FTSEStockDataPath, FTSET33FileName);
    FTSEO40FilePathName       = ComposeFilePathName(FTSEStockDataPath, FTSEO40FileName);
    FTSEO60FilePathName       = ComposeFilePathName(FTSEStockDataPath, FTSEO60FileName);
    FOTCT30FormatFilePathName = ComposeFilePathName(FFormatFilePath, FOTCT30FormatFileName);
    FOTCT32FormatFilePathName = ComposeFilePathName(FFormatFilePath, FOTCT32FormatFileName);
    FOTCT33FormatFilePathName = ComposeFilePathName(FFormatFilePath, FOTCT33FormatFileName);
    FOTCO40FormatFilePathName = ComposeFilePathName(FFormatFilePath, FOTCO40FormatFileName);
    FOTCO60FormatFilePathName = ComposeFilePathName(FFormatFilePath, FOTCO60FormatFileName);
    FOTCT30FilePathName       = ComposeFilePathName(FOTCStockDataPath, FOTCT30FileName);
    FOTCT32FilePathName       = ComposeFilePathName(FOTCStockDataPath, FOTCT32FileName);
    FOTCT33FilePathName       = ComposeFilePathName(FOTCStockDataPath, FOTCT33FileName);
    FOTCO40FilePathName       = ComposeFilePathName(FOTCStockDataPath, FOTCO40FileName);
    FOTCO60FilePathName       = ComposeFilePathName(FOTCStockDataPath, FOTCO60FileName);
}  //CRiskManagerDataPool::LoadSetting()
//------------------------------------------------------------------------------
void CRiskManagerDataPool::PrintSetting()
{
    FLogPtr->fprintf(" ");
    FLogPtr->fprintf(" ");
    FLogPtr->fprintf(" ");
    FLogPtr->fprintf(" ");
    FLogPtr->fprintf(" ");
    FLogPtr->fprintf(" ______________________________________________");
    if (FIsRecoverMode)
        FLogPtr->fprintf(" %s Start Up at: %s, RecoverMode on.", FRiskManagerID.c_str(), UFC::Hostname);
    else
        FLogPtr->fprintf(" %s Start Up at: %s, RecoverMode off.", FRiskManagerID.c_str(), UFC::Hostname);
    
    FLogPtr->fprintf(" Build: %s %s, %ld bit mode", __DATE__, __TIME__, sizeof(void*) * 8);
    FLogPtr->fprintf(" Configure File[%s]", FConfigureFilePathName.c_str());
    
    FLogPtr->fprintf(" TSE T30 Format[%s] File[%s]", FTSET30FormatFilePathName.c_str(), FTSET30FilePathName.c_str());
    FLogPtr->fprintf(" TSE T32 Format[%s] File[%s]", FTSET32FormatFilePathName.c_str(), FTSET32FilePathName.c_str());
    FLogPtr->fprintf(" TSE T33 Format[%s] File[%s]", FTSET33FormatFilePathName.c_str(), FTSET33FilePathName.c_str());
    FLogPtr->fprintf(" TSE O40 Format[%s] File[%s]", FTSEO40FormatFilePathName.c_str(), FTSEO40FilePathName.c_str());
    FLogPtr->fprintf(" TSE O60 Format[%s] File[%s]", FTSEO60FormatFilePathName.c_str(), FTSEO60FilePathName.c_str());
    FLogPtr->fprintf(" OTC T30 Format[%s] File[%s]", FOTCT30FormatFilePathName.c_str(), FOTCT30FilePathName.c_str());
    FLogPtr->fprintf(" OTC T32 Format[%s] File[%s]", FOTCT32FormatFilePathName.c_str(), FOTCT32FilePathName.c_str());
    FLogPtr->fprintf(" OTC T33 Format[%s] File[%s]", FOTCT33FormatFilePathName.c_str(), FOTCT33FilePathName.c_str());
    FLogPtr->fprintf(" OTC O40 Format[%s] File[%s]", FOTCO40FormatFilePathName.c_str(), FOTCO40FilePathName.c_str());
    FLogPtr->fprintf(" OTC O60 Format[%s] File[%s]", FOTCO60FormatFilePathName.c_str(), FOTCO60FilePathName.c_str());
    
    FLogPtr->fprintf(" ______________________________________________");
    FLogPtr->Flush();
}  //CRiskManagerDataPool::PrintSetting()

};  //namespace twserm

