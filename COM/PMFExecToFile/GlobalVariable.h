/* 
 * File:   GlobalVariable.h
 * Author: Zhen Fan
 *
 * Created on 2021/03/18 10:01
 */

#ifndef GLOBALVARIABLE_H
#define GLOBALVARIABLE_H
#include "UFC.h"

extern bool            gIsFirstTimeStartup;
extern bool            gDumpOnly;
extern UFC::UDateTime  gAppStartUpTime;
extern UFC::AnsiString gRecoverTimeFileDirectory;
extern UFC::AnsiString gRecoverTimeFileName;
extern UFC::AnsiString gRecoverTimeFilePathName;
extern UFC::PInt32     gRecoverBeginTime;
extern UFC::UTime      gRecoverTime;
extern UFC::AnsiString gResendTimeStr;

extern UFC::AnsiString gConfigureFileDirectory;
extern UFC::AnsiString gConfigureFileName;
extern UFC::AnsiString gConfigureFilePathName;
extern UFC::AnsiString gExecRptFileDate;

extern UFC::AnsiString gAppName;
extern UFC::AnsiString gAppVersion;
extern UFC::AnsiString gAppDescription;
extern int             gAppID;
extern UFC::AnsiString gMBusIP;
extern int             gMBusPort;
extern UFC::AnsiString gMBusHostName;
extern UFC::AnsiString gGatewayIP;
extern int             gGatewayPort;
extern UFC::AnsiString gGatewayID;
extern UFC::AnsiString gGatewayPassword;
extern UFC::AnsiString gGatewayAccount;
extern UFC::AnsiString gExecRptFileDir;

extern UFC::AnsiString gFUTFileNamePrefix;
extern UFC::AnsiString gFUTFilePathName;
extern UFC::AnsiString gOPTFileNamePrefix;
extern UFC::AnsiString gOPTFilePathName;
extern UFC::AnsiString gPATSFileNamePrefix;
extern UFC::AnsiString gPATSConfirmFilePathName;
extern UFC::AnsiString gPATSFillFilePathName;
extern UFC::AnsiString gStockFileNamePrefix;
extern UFC::AnsiString gStockConfirmFilePathName;
extern UFC::AnsiString gStockFillFilePathName;

extern UFC::AnsiString gDefaultAE;

void gLoadSetting();
void gPrintSetting();
void gGetAE( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Acc, const UFC::AnsiString& UserInformation, UFC::AnsiString& AE );
#endif /* GLOBALVARIABLE_H */
