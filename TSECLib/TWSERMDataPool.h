/* 
 * File:   TWSERMDataPool.h
 * Author: Zhen Fan
 *
 * Created on 2021/01/06 15:38
 */

#ifndef TWSERMDATAPOOL_H
#define TWSERMDATAPOOL_H

#include "../UFC/UFC.h"                //UFC
#include "../UFC/ContainerTemplate.h"  //UFC
#include "../COM/Lib/OrderMessage/OrderMessageDefine.h"  //COM/Lib
#include "../imFIXMapper/SecurityInfoMap.h"  //imFixMapper

namespace twserm
{
//------------------------------------------------------------------------------
class CRiskManagerDataPool
{
protected:
    static nsOrderMessageDefine::MarketEnum FMarket;
    static UFC::AnsiString                  FMarketName;
    
    static UFC::AnsiString FRiskManagerID;
    static UFC::AnsiString FConfigureFilePathName;
    
    static UFC::AnsiString FLogFilePath;
    static UFC::AnsiString FFormatFilePath;
    static UFC::AnsiString FTSEStockDataPath;
    static UFC::AnsiString FOTCStockDataPath;

    static UFC::AnsiString FTSET30FormatFileName;
    static UFC::AnsiString FTSET32FormatFileName;
    static UFC::AnsiString FTSET33FormatFileName;
    static UFC::AnsiString FTSEO40FormatFileName;
    static UFC::AnsiString FTSEO60FormatFileName;
    static UFC::AnsiString FTSET30FileName;
    static UFC::AnsiString FTSET32FileName;
    static UFC::AnsiString FTSET33FileName;
    static UFC::AnsiString FTSEO40FileName;
    static UFC::AnsiString FTSEO60FileName;

    static UFC::AnsiString FOTCT30FormatFileName;
    static UFC::AnsiString FOTCT32FormatFileName;
    static UFC::AnsiString FOTCT33FormatFileName;
    static UFC::AnsiString FOTCO40FormatFileName;
    static UFC::AnsiString FOTCO60FormatFileName;
    static UFC::AnsiString FOTCT30FileName;
    static UFC::AnsiString FOTCT32FileName;
    static UFC::AnsiString FOTCT33FileName;
    static UFC::AnsiString FOTCO40FileName;
    static UFC::AnsiString FOTCO60FileName;

    static UFC::AnsiString FTSET30FormatFilePathName;
    static UFC::AnsiString FTSET32FormatFilePathName;
    static UFC::AnsiString FTSET33FormatFilePathName;
    static UFC::AnsiString FTSEO40FormatFilePathName;
    static UFC::AnsiString FTSEO60FormatFilePathName;
    static UFC::AnsiString FTSET30FilePathName;
    static UFC::AnsiString FTSET32FilePathName;
    static UFC::AnsiString FTSET33FilePathName;
    static UFC::AnsiString FTSEO40FilePathName;
    static UFC::AnsiString FTSEO60FilePathName;
    static UFC::AnsiString FOTCT30FormatFilePathName;
    static UFC::AnsiString FOTCT32FormatFilePathName;
    static UFC::AnsiString FOTCT33FormatFilePathName;
    static UFC::AnsiString FOTCO40FormatFilePathName;
    static UFC::AnsiString FOTCO60FormatFilePathName;
    static UFC::AnsiString FOTCT30FilePathName;
    static UFC::AnsiString FOTCT32FilePathName;
    static UFC::AnsiString FOTCT33FilePathName;
    static UFC::AnsiString FOTCO40FilePathName;
    static UFC::AnsiString FOTCO60FilePathName;
    
    static bool FIsTSET30Ready;
    static bool FIsTSET32Ready;
    static bool FIsTSET33Ready;
    static bool FIsTSEO40Ready;
    static bool FIsTSEO60Ready;
    static bool FIsOTCT30Ready;
    static bool FIsOTCT32Ready;
    static bool FIsOTCT33Ready;
    static bool FIsOTCO40Ready;
    static bool FIsOTCO60Ready;
    
    static UFC::BufferedLog* FLogPtr;
    static bool FIsRecoverMode;
    
    static SecurityInfoMap FStockMap;
    
public:
    static nsOrderMessageDefine::MarketEnum GetMarket() {return FMarket;};
    static void SetMarket(nsOrderMessageDefine::MarketEnum NewValue) {FMarket = NewValue;};
    
    static UFC::AnsiString GetMarketName() {return FMarketName;};
    static void SetMarketName(const UFC::AnsiString& NewValue) {FMarketName = NewValue;};
    
    static UFC::AnsiString GetRiskManagerID() {return FRiskManagerID;};
    static void SetRiskManagerID(const UFC::AnsiString& NewValue) {FRiskManagerID = NewValue;};
    
    static UFC::AnsiString GetConfigureFilePathName() {return FConfigureFilePathName;};
    static void SetConfigureFilePathName(const UFC::AnsiString& NewValue) {FConfigureFilePathName = NewValue;};
    
    static UFC::AnsiString GetLogFilePath() {return FLogFilePath;};
    static void SetLogFilePath(const UFC::AnsiString& NewValue) {FLogFilePath = NewValue;};
    
    static UFC::AnsiString GetFormatFilePath() {return FFormatFilePath;};
    static void SetFormatFilePath(const UFC::AnsiString& NewValue) {FFormatFilePath = NewValue;};
    
    static UFC::AnsiString GetTSEStockDataPath() {return FTSEStockDataPath;};
    static void SetTSEStockDataPath(const UFC::AnsiString& NewValue) {FTSEStockDataPath = NewValue;};

    static UFC::AnsiString GetOTCStockDataPath() {return FOTCStockDataPath;};
    static void SetOTCStockDataPath(const UFC::AnsiString& NewValue) {FOTCStockDataPath = NewValue;};

    static UFC::AnsiString GetTSET30FormatFileName() {return FTSET30FormatFileName;};
    static void SetTSET30FormatFileName(const UFC::AnsiString& NewValue) {FTSET30FormatFileName = NewValue;};
    
    static UFC::AnsiString GetTSET32FormatFileName() {return FTSET32FormatFileName;};
    static void SetTSET32FormatFileName(const UFC::AnsiString& NewValue) {FTSET32FormatFileName = NewValue;};
    
    static UFC::AnsiString GetTSET33FormatFileName() {return FTSET33FormatFileName;};
    static void SetTSET33FormatFileName(const UFC::AnsiString& NewValue) {FTSET33FormatFileName = NewValue;};
    
    static UFC::AnsiString GetTSEO40FormatFileName() {return FTSEO40FormatFileName;};
    static void SetTSEO40FormatFileName(const UFC::AnsiString& NewValue) {FTSEO40FormatFileName = NewValue;};
    
    static UFC::AnsiString GetTSEO60FormatFileName() {return FTSEO60FormatFileName;};
    static void SetTSEO60FormatFileName(const UFC::AnsiString& NewValue) {FTSEO60FormatFileName = NewValue;};
    
    static UFC::AnsiString GetTSET30FileName() {return FTSET30FileName;};
    static void SetTSET30FileName(const UFC::AnsiString& NewValue) {FTSET30FileName = NewValue;};
    
    static UFC::AnsiString GetTSET32FileName() {return FTSET32FileName;};
    static void SetTSET32FileName(const UFC::AnsiString& NewValue) {FTSET32FileName = NewValue;};
    
    static UFC::AnsiString GetTSET33FileName() {return FTSET33FileName;};
    static void SetTSET33FileName(const UFC::AnsiString& NewValue) {FTSET33FileName = NewValue;};
    
    static UFC::AnsiString GetTSEO40FileName() {return FTSEO40FileName;};
    static void SetTSEO40FileName(const UFC::AnsiString& NewValue) {FTSEO40FileName = NewValue;};
    
    static UFC::AnsiString GetTSEO60FileName() {return FTSEO60FileName;};
    static void SetTSEO60FileName(const UFC::AnsiString& NewValue) {FTSEO60FileName = NewValue;};

    static UFC::AnsiString GetOTCT30FormatFileName() {return FOTCT30FormatFileName;};
    static void SetOTCT30FormatFileName(const UFC::AnsiString& NewValue) {FOTCT30FormatFileName = NewValue;};
    
    static UFC::AnsiString GetOTCT32FormatFileName() {return FOTCT32FormatFileName;};
    static void SetOTCT32FormatFileName(const UFC::AnsiString& NewValue) {FOTCT32FormatFileName = NewValue;};
    
    static UFC::AnsiString GetOTCT33FormatFileName() {return FOTCT33FormatFileName;};
    static void SetOTCT33FormatFileName(const UFC::AnsiString& NewValue) {FOTCT33FormatFileName = NewValue;};
    
    static UFC::AnsiString GetOTCO40FormatFileName() {return FOTCO40FormatFileName;};
    static void SetOTCO40FormatFileName(const UFC::AnsiString& NewValue) {FOTCO40FormatFileName = NewValue;};
    
    static UFC::AnsiString GetOTCO60FormatFileName() {return FOTCO60FormatFileName;};
    static void SetOTCO60FormatFileName(const UFC::AnsiString& NewValue) {FOTCO60FormatFileName = NewValue;};
    
    static UFC::AnsiString GetOTCT30FileName() {return FOTCT30FileName;};
    static void SetOTCT30FileName(const UFC::AnsiString& NewValue) {FOTCT30FileName = NewValue;};
    
    static UFC::AnsiString GetOTCT32FileName() {return FOTCT32FileName;};
    static void SetOTCT32FileName(const UFC::AnsiString& NewValue) {FOTCT32FileName = NewValue;};
    
    static UFC::AnsiString GetOTCT33FileName() {return FOTCT33FileName;};
    static void SetOTCT33FileName(const UFC::AnsiString& NewValue) {FOTCT33FileName = NewValue;};
    
    static UFC::AnsiString GetOTCO40FileName() {return FOTCO40FileName;};
    static void SetOTCO40FileName(const UFC::AnsiString& NewValue) {FOTCO40FileName = NewValue;};
    
    static UFC::AnsiString GetOTCO60FileName() {return FOTCO60FileName;};
    static void SetOTCO60FileName(const UFC::AnsiString& NewValue) {FOTCO60FileName = NewValue;};
    
    static bool IsTSET30Ready() {return FIsTSET30Ready;};
    static void SetTSET30Ready(bool NewValue) {FIsTSET30Ready = NewValue;};
    
    static bool IsTSET32Ready() {return FIsTSET32Ready;};
    static void SetTSET32Ready(bool NewValue) {FIsTSET32Ready = NewValue;};
    
    static bool IsTSET33Ready() {return FIsTSET33Ready;};
    static void SetTSET33Ready(bool NewValue) {FIsTSET33Ready = NewValue;};
    
    static bool IsTSEO40Ready() {return FIsTSEO40Ready;};
    static void SetTSEO40Ready(bool NewValue) {FIsTSEO40Ready = NewValue;};
    
    static bool IsTSEO60Ready() {return FIsTSEO60Ready;};
    static void SetTSEO60Ready(bool NewValue) {FIsTSEO60Ready = NewValue;};
    
    static bool IsOTCT30Ready() {return FIsOTCT30Ready;};
    static void SetOTCT30Ready(bool NewValue) {FIsOTCT30Ready = NewValue;};
    
    static bool IsOTCT32Ready() {return FIsOTCT32Ready;};
    static void SetOTCT32Ready(bool NewValue) {FIsOTCT32Ready = NewValue;};
    
    static bool IsOTCT33Ready() {return FIsOTCT33Ready;};
    static void SetOTCT33Ready(bool NewValue) {FIsOTCT33Ready = NewValue;};
    
    static bool IsOTCO40Ready() {return FIsOTCO40Ready;};
    static void SetOTCO40Ready(bool NewValue) {FIsOTCO40Ready = NewValue;};
    
    static bool IsOTCO60Ready() {return FIsOTCO60Ready;};
    static void SetOTCO60Ready(bool NewValue) {FIsOTCO60Ready = NewValue;};

    static UFC::AnsiString ComposeFilePathName(const UFC::AnsiString& FilePath, const UFC::AnsiString& FileName);
    
    static UFC::BufferedLog* CreateLogObject();
    static void DeleteLogObject();
    static UFC::BufferedLog* GetLogObject() {return FLogPtr;};
    static void SetLogObject(UFC::BufferedLog* LogPtr) {FLogPtr = LogPtr;};
    
    static bool IsRecoverMode() {return FIsRecoverMode;};
    static void SetRecoverMode(bool RecoverMode) {FIsRecoverMode = RecoverMode;};
    
    static bool LoadTSET30File();
    static bool LoadTSET32File();
    static bool LoadTSET33File();
    static bool LoadTSEO40File();
    static bool LoadTSEO60File();
    static bool LoadOTCT30File();
    static bool LoadOTCT32File();
    static bool LoadOTCT33File();
    static bool LoadOTCO40File();
    static bool LoadOTCO60File();
    static void LoadStockDataFile();
    static void DumpStockData();

    static SecurityInfo* FindStock(const UFC::AnsiString& StockNo);
    
protected:
    
public:
    CRiskManagerDataPool();
    virtual ~CRiskManagerDataPool();
    
    virtual void LoadSetting();
    virtual void PrintSetting();
};  //CRiskManagerDataPool

extern CRiskManagerDataPool* gRMDataPoolPtr;
}  //namespace twserm
#endif /* TWSERMDATAPOOL_H */

