/* 
 * File:   GlobalVariable.cpp
 * Author: Zhen Fan
 *
 * Created on 2021/03/18 10:01
 */

#include "iniFile.h"
#include "GlobalVariable.h"
//------------------------------------------------------------------------------
bool            gIsFirstTimeStartup = false;
bool            gDumpOnly = false;
UFC::UDateTime  gAppStartUpTime;
UFC::AnsiString gRecoverTimeFileDirectory = "../log";
UFC::AnsiString gRecoverTimeFileName = "";
UFC::AnsiString gRecoverTimeFilePathName = "../log/PMFExecToFile";
UFC::PInt32     gRecoverBeginTime;
UFC::UTime      gRecoverTime;
UFC::AnsiString gResendTimeStr = "";


UFC::AnsiString gConfigureFileDirectory = "../cfg";
UFC::AnsiString gConfigureFileName      = "PMFExecToFile.cfg";
UFC::AnsiString gConfigureFilePathName  = "../cfg/PMFExecToFile.cfg";
UFC::AnsiString gExecRptFileDate = "";
        
UFC::AnsiString gAppName        = "PMFExecToFile";
UFC::AnsiString gAppVersion     = "1.0.0";
UFC::AnsiString gAppDescription = "WLS Execution Report to File";
int             gAppID          = 1;
UFC::AnsiString gMBusIP         = "127.0.0.1";
int             gMBusPort       = 12345;
UFC::AnsiString gMBusHostName   = "";

UFC::AnsiString gGatewayIP       = "127.0.0.1";
int             gGatewayPort     = 23456;
UFC::AnsiString gGatewayID       = "yuan";
UFC::AnsiString gGatewayPassword = "1234rewq";
UFC::AnsiString gGatewayAccount  = "0000000";
UFC::AnsiString gExecRptFileDir  = "/Speedy/ulog";
int             gTWSEFormat      = 40;

UFC::AnsiString gFUTFileNamePrefix       = "FUTExec";
UFC::AnsiString gFUTFilePathName         = "";
UFC::AnsiString gOPTFileNamePrefix       = "OPTExec";
UFC::AnsiString gOPTFilePathName         = "";
UFC::AnsiString gPATSFileNamePrefix      = "PATSExec";
UFC::AnsiString gPATSConfirmFilePathName = "";
UFC::AnsiString gPATSFillFilePathName    = "";

UFC::AnsiString gStockFileNamePrefix      = "StockExec";
UFC::AnsiString gStockConfirmFilePathName = "";
UFC::AnsiString gStockFillFilePathName    = "";

UFC::AnsiString gDefaultAE = "00 ";

UFC::PHashMap<UFC::AnsiString, UFC::AnsiString*> gAEMap;
//------------------------------------------------------------------------------
void gGetAE( const UFC::AnsiString& BrokerID, const UFC::AnsiString& Acc, const UFC::AnsiString& UserInformation, UFC::AnsiString& AE )
{
    UFC::AnsiString  Key = BrokerID + Acc;
    UFC::AnsiString* pAE;
    if( gAEMap.GetObjectByKey( Key, pAE ) == TRUE )
    {
        AE = UFC::AnsiString( pAE->c_str() );
        UFC::BufferedLog::Printf( " %s() [%s][%s] ---> AE[%s]", __func__, BrokerID.c_str(), Acc.c_str(), AE.c_str() );
    }
    else
    {
        int userInfoLength = UserInformation.Length();
        if( userInfoLength > 0 )
        {
            int caretPos = UserInformation.AnsiPos( "^" );
            if( caretPos > 0 )
            {
                AE = UserInformation.SubString( 0, caretPos );
                UFC::BufferedLog::Printf( " %s() Get AE[%s] from UserData[%s].", __func__, AE.c_str(), UserInformation.c_str() );
            }
            else
                AE = "";
        }
        else
            AE = "";
    }  //if( gAEMap.GetObjectByKey( Key, pAE ) == TRUE )
    
    if( AE.Length() <= 0 )
    {
        AE = gDefaultAE;
        UFC::BufferedLog::Printf( " %s() [%s][%s] ---> Default AE[%s]", __func__, BrokerID.c_str(), Acc.c_str(), gDefaultAE.c_str() );                
    }
}  //gGetAE()
//------------------------------------------------------------------------------
void gLoadSCUST()
{
    try
    {
        UFC::FileStreamEx DMAUserFile( "./SCUST.TXT", "r" );
        UFC::AnsiString  Line; 

        if( DMAUserFile.ReadLine( Line ) == true )
        {
            UFC::AnsiString Header( Line.c_str(), 5 ); 
            UFC::AnsiString Date( Line.c_str()+ 10, 8 ); 

            if( Header == "SCUST" )
            {
                UFC::BufferedLog::Printf( " SCUST file date[%s]", Date.c_str() );
                while( DMAUserFile.ReadLine( Line ) == true )
                {
                    if( Line.AnsiPos( "0000000000000") == 0 )
                    {
                        UFC::BufferedLog::Printf( " END of SCUST file." );
                        return;
                    }
                    else
                    {
                        UFC::AnsiString Key( Line.c_str(), 11 );
                        UFC::AnsiString BrokerID( Line.c_str(), 4 );
                        UFC::AnsiString Account( Line.c_str() + 4, 7 );                    
                        UFC::AnsiString AE( Line.c_str() + 24, 3 );

                        gAEMap.Add( Key , new UFC::AnsiString( AE ) ); 

                        UFC::BufferedLog::Printf( " [%s][%s] use AE[%s]", BrokerID.c_str(),Account.c_str(),AE.c_str()  );                
                    }   
                }
            }
            else
                UFC::BufferedLog::Printf( " Not a valid SCUST file." );
        }
    }
    catch(...)
    {
        UFC::BufferedLog::Printf( " SCUST.TXT not exist." );
        UFC::BufferedLog::Printf( " All accounts use default AE[%s]", gDefaultAE.c_str() );
    }
}
//------------------------------------------------------------------------------
void gLoadSetting()
{
    UFC::AnsiString logHead = "gLoadSetting()";
    gMBusHostName  = UFC::Hostname;
    
    UFC::UiniFile *configFilePtr = 0;
    try
    {
        configFilePtr = new UFC::UiniFile(gConfigureFilePathName);
    }
    catch(UFC::FileException& fe)
    {
        UFC::BufferedLog::Printf(" %s Load Configure File[%s] failed. %s", logHead.c_str(), gConfigureFilePathName.c_str(), fe.what());
        exit(1);
    }

    UFC::AnsiString sectionName = "Setting";
    UFC::AnsiString fieldName = "AppName";
    UFC::AnsiString fieldValue = "";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gAppName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gAppName.c_str());
    
    fieldName = "AppDescription";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gAppDescription = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gAppDescription.c_str());
    
    fieldName = "AppID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gAppID = fieldValue.ToInt();
    
    fieldName = "MBusIP";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gMBusIP = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gMBusIP.c_str());

    fieldName = "MBusPort";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gMBusPort = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%d]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gMBusPort);

    fieldName = "MBusHostName";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gMBusHostName = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gMBusHostName.c_str());

    fieldName = "GatewayIP";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gGatewayIP = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gGatewayIP.c_str());

    fieldName = "GatewayPort";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gGatewayPort = fieldValue.ToInt();
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%d]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gGatewayPort);

    fieldName = "GatewayID";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gGatewayID = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gGatewayID.c_str());

    fieldName = "GatewayPassword";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gGatewayPassword = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gGatewayPassword.c_str());
    
    fieldName = "GatewayAccount";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gGatewayAccount = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gGatewayAccount.c_str());
    
    fieldName = "ExecRptFileDir";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gExecRptFileDir = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gExecRptFileDir.c_str());
    
//    fieldName = "TWSEFormat";
//    if( configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE && fieldValue == "30" )
//        gTWSEFormat = 30;
//    else
//        gTWSEFormat = 40;
    
    fieldName = "DefaultAE";
    configFilePtr->GetValue(sectionName, fieldName, gDefaultAE);
        
    sectionName = "DefFiles";
    fieldName = "FUTPrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gFUTFileNamePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gFUTFileNamePrefix.c_str());

    fieldName = "OPTPrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gOPTFileNamePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gOPTFileNamePrefix.c_str());

    fieldName = "PATSPrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gPATSFileNamePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gPATSFileNamePrefix.c_str());
    
    fieldName = "StockPrefix";
    if (configFilePtr->GetValue(sectionName, fieldName, fieldValue) == TRUE)
        gStockFileNamePrefix = fieldValue;
    else
        UFC::BufferedLog::Printf(" %s [%s-%s] not Setting, use Default Value:[%s]", logHead.c_str(), sectionName.c_str(), fieldName.c_str(), gStockFileNamePrefix.c_str());    
    
    delete configFilePtr;
    
    if (gRecoverTimeFileName.Length() <= 0) gRecoverTimeFileName = gAppName;
    gRecoverTimeFilePathName.Printf("%s/%s", gRecoverTimeFileDirectory.c_str(), gRecoverTimeFileName.c_str());
    
    Int32 todayNumber = 0;
    if (gExecRptFileDate.Length() == 7) 
        todayNumber = gExecRptFileDate.ToInt();
    else
        todayNumber = UFC::GetDateString().ToInt();
    gFUTFilePathName.Printf("%s/%s.%d", gExecRptFileDir.c_str(), gFUTFileNamePrefix.c_str(), todayNumber);
    gOPTFilePathName.Printf("%s/%s.%d", gExecRptFileDir.c_str(), gOPTFileNamePrefix.c_str(), todayNumber);
    gPATSConfirmFilePathName.Printf("%s/%sConfirm.%d", gExecRptFileDir.c_str(), gPATSFileNamePrefix.c_str(), todayNumber);
    gPATSFillFilePathName.Printf("%s/%sFill.%d", gExecRptFileDir.c_str(), gPATSFileNamePrefix.c_str(), todayNumber);    
    
    gStockConfirmFilePathName.Printf("%s/%sConfirm.%d", gExecRptFileDir.c_str(), gStockFileNamePrefix.c_str(), todayNumber);
    gStockFillFilePathName.Printf("%s/%sFill.%d", gExecRptFileDir.c_str(), gStockFileNamePrefix.c_str(), todayNumber);
    
    gLoadSCUST();
}  //gLoadSetting()
//------------------------------------------------------------------------------
void gPrintSetting()
{
    UFC::BufferedLog::Printf("______________________________________________");
    UFC::BufferedLog::Printf(" PMFExecToFile Copyright (c) September 2009~%04d MDBS, Build: %s %s, %ld bit mode", gAppStartUpTime.getYear(), __DATE__, __TIME__, sizeof(void*) * 8);
    UFC::BufferedLog::Printf("                                              ");
    if (gIsFirstTimeStartup)
        UFC::BufferedLog::Printf(" %s First Time Start Up at: %s.", gAppName.c_str(), UFC::Hostname);
    else
        UFC::BufferedLog::Printf(" %s Start Up at: %s.", gAppName.c_str(), UFC::Hostname);
    UFC::BufferedLog::Printf(" Application Name[%s] ID[%d] Version[%s] Description[%s]", gAppName.c_str(), gAppID, gAppVersion.c_str(), gAppDescription.c_str());
    UFC::BufferedLog::Printf(" Configure File[%s]", gConfigureFilePathName.c_str());
    UFC::BufferedLog::Printf("                                          ");
    UFC::BufferedLog::Printf(" MBus Server[%s] Address[%s:%d]", gMBusHostName.c_str(), gMBusIP.c_str(), gMBusPort);
    UFC::BufferedLog::Printf(" Gateway Server[%s:%d] ID[%s] Account[%s]", gGatewayIP.c_str(), gGatewayPort, gGatewayID.c_str(), gGatewayAccount.c_str());
    
    UFC::BufferedLog::Printf(" Recover Time File[%s]", gRecoverTimeFilePathName.c_str());
    UFC::BufferedLog::Printf(" Future File[%s]", gFUTFilePathName.c_str());
    UFC::BufferedLog::Printf(" Option File[%s]", gOPTFilePathName.c_str());
    UFC::BufferedLog::Printf(" PATS  Confirm File[%s]", gPATSConfirmFilePathName.c_str());
    UFC::BufferedLog::Printf(" PATS  Fill    File[%s]", gPATSFillFilePathName.c_str());
    UFC::BufferedLog::Printf(" Stock Confirm File[%s]", gStockConfirmFilePathName.c_str());
    UFC::BufferedLog::Printf(" Stock Fill    File[%s]", gStockFillFilePathName.c_str());
    UFC::BufferedLog::Printf(" Default AE[%s]", gDefaultAE.c_str());
//    if( gTWSEFormat == 30 )
//        UFC::BufferedLog::Printf(" Stock File use format version[30]" );
//    else
//        UFC::BufferedLog::Printf(" Stock File use format version[3a]" );
    
    UFC::BufferedLog::Printf("______________________________________________");
    UFC::BufferedLog::Printf(" ");
    UFC::BufferedLog::FlushToFile();
}  //gPrintSetting()
//------------------------------------------------------------------------------
