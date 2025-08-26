/* 
 * File:   MBusAdapter.cpp
 * Author: Zhen Fan
 *
 * Created on 2011/12/15 10:01
 */
#include "MBusAdapter.h"
#include "iniFile.h"

namespace jihsun
{
//-------------------------------- CMBusAdapter --------------------------------
//------------------------------------------------------------------------------
CMBusAdapter::CMBusAdapter( const UFC::AnsiString& ConfigFileName )
:FConfigFileName(ConfigFileName),
 FAppName("JSExecConnection"),
 FAppVersion("1.0.0"),
 FAppDescription("Captial Execution Connection TCP"),
 FMBusIP("127.0.0.1"),
 FMBusPort(12345),
 FAlertSubject("SPEEDY.COMMAND"),
 FAlertKey(UFC::Hostname),
 FNoticeSubject("FromBackend"),
 FNoticeKey(UFC::Hostname),
 FMsgObjPtr(0)
{
    LoadSetting();
    PrintSetting();
    
    FMsgObjPtr = new MessageObject(FAppName, FAppVersion, FAppDescription, FMBusPort);
    FMsgObjPtr->SetHost(FMBusIP);
    FMsgObjPtr->SetMonitorListener(this);
    FMsgObjPtr->Start();
    FMsgObjPtr->WaitForConnected();    
}  //CMBusAdapter::CMBusAdapter()    
//------------------------------------------------------------------------------
CMBusAdapter::~CMBusAdapter()
{
    if (FMsgObjPtr != 0) delete FMsgObjPtr;
}  //CMBusAdapter::~CMBusAdapter()

//------------------------------------------------------------------------------
void CMBusAdapter::LoadSetting()
{
    UFC::UiniFile *ConfigFilePtr = 0;
    try 
    {
        ConfigFilePtr = new UFC::UiniFile(FConfigFileName);
    }
    catch (UFC::FileException fe)
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() [%s] Error: %s", FConfigFileName.c_str(), fe.what());
        ConfigFilePtr = NULL;
    }
    
    if (ConfigFilePtr != NULL)
    {
        UFC::AnsiString TempStr;
        if (ConfigFilePtr->GetValue("Setting", "AppName", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Application Name is Not Setting in ConfigFile, Use Default Value \"%s\".", FAppName.c_str());
        else
            FAppName = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "AppVersion", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Application Version is Not Setting in ConfigFile, Use Default Value \"%s\".", FAppVersion.c_str());
        else
            FAppVersion = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "AppDescription", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Application Description is Not Setting in ConfigFile, Use Default Value \"%s\".", FAppDescription.c_str());
        else
            FAppDescription = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "MBusIP", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() MBus IP is Not Setting in ConfigFile, Use Default Value \"%s\".", FMBusIP.c_str());
        else
            FMBusIP = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "MBusPort", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() MBus Port is Not Setting in ConfigFile, Use Default Value %d!", FMBusPort);
        else
            FMBusPort = TempStr.ToInt();

        if (ConfigFilePtr->GetValue("Setting", "AlertSubject", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Alert Subject is Not Setting in ConfigFile, Use Default Value \"%s\"!", FAlertSubject.c_str());
        else
            FAlertSubject = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "AlertKey", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Alert Key is Not Setting in ConfigFile, Use Default Value \"%s\"!", FAlertKey.c_str());
        else
            FAlertKey = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "NoticeSubject", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Notice Subject is Not Setting in ConfigFile, Use Default Value \"%s\"!", FNoticeSubject.c_str());
        else
            FNoticeSubject = TempStr;

        if (ConfigFilePtr->GetValue("Setting", "NoticeKey", TempStr) == FALSE)
            UFC::BufferedLog::Printf(" CMBusAdapter::LoadSetting() Notice Key is Not Setting in ConfigFile, Use Default Value \"%s\"!", FNoticeKey.c_str());
        else
            FNoticeKey = TempStr;
        
        delete ConfigFilePtr;
    }  //if (ConfigFilePtr != NULL)
    UFC::BufferedLog::FlushToFile();    
}  //CMBusAdapter::LoadSetting()
//------------------------------------------------------------------------------
void CMBusAdapter::PrintSetting()
{
    UFC::BufferedLog::Printf(" -------------------------------  CMBusAdapter  -------------------------------");
    UFC::BufferedLog::Printf(" [%s] ver[%s] : [%s]", FAppName.c_str(), FAppVersion.c_str(), FAppDescription.c_str());
    UFC::BufferedLog::Printf(" MBus IP:Port [%s:%d]", FMBusIP.c_str(), FMBusPort);
    UFC::BufferedLog::Printf(" Alert [%s:%s]", FAlertSubject.c_str(), FAlertKey.c_str());
    UFC::BufferedLog::Printf(" Notice [%s:%s]", FNoticeSubject.c_str(), FNoticeKey.c_str());
    UFC::BufferedLog::Printf(" -------------------------------  CMBusAdapter  -------------------------------");
    UFC::BufferedLog::Printf(" ");
    UFC::BufferedLog::FlushToFile();    
}  //CMBusAdapter::PrintSetting()
//------------------------------------------------------------------------------
bool CMBusAdapter::IsTerminate()
{
    if (FMsgObjPtr == 0) 
        return true;
    else
        return FMsgObjPtr->IsTerminated();
}  //CMBusAdapter::IsTerminate()
//------------------------------------------------------------------------------
bool CMBusAdapter::SendAlert(const UFC::AnsiString& AlertMessage)
{
    MTree Data;
    Data.append("COMMAND", 9020);
    Data.append("MSG", AlertMessage);
    if (FMsgObjPtr->Send(FAlertSubject, FAlertKey, Data, FALSE) == TRUE)
        return true;
    else
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::SendAlert() Send Alert Message[%s] failed.", AlertMessage.c_str());
        return false;        
    }
}  //CMBusAdapter::SendAlert()
//------------------------------------------------------------------------------
bool CMBusAdapter::SendNotice(const UFC::AnsiString& NoticeMessage)
{
    MTree Data;
    Data.append("Data", NoticeMessage.c_str());
    if (FMsgObjPtr->Send(FNoticeSubject, FNoticeKey, Data, FALSE) == TRUE)
        return true;
    else
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::SendNotice() Send Notice Message[%s] failed.", NoticeMessage.c_str());
        return false;        
    }
}  //CMBusAdapter::SendAlert()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessStartup(const UFC::AnsiString& Host, const UFC::AnsiString& AppName)
{
    UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessStartup Host[%s] App[%s]", Host.c_str(), AppName.c_str());        
}  //CMBusAdapter::OnProcessStartup()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessStopped(const UFC::AnsiString& Host, const UFC::AnsiString& AppName)
{
    UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessStopped() Host[%s] App[%s]", Host.c_str(), AppName.c_str());    
}  //CMBusAdapter::OnProcessStopped()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessConnected(BOOL IsTheFirstOne)
{
    if (IsTheFirstOne == TRUE)
        UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessConnected() %s is First Process on %s:%d.", FAppName.c_str(), FMBusIP.c_str(), FMBusPort);
    else
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessConnected() %s already exists on %s:%d.", FAppName.c_str(), FMBusIP.c_str(), FMBusPort);
        FMsgObjPtr->Terminate();
    }
}  //CMBusAdapter::OnProcessConnected()
//------------------------------------------------------------------------------
void CMBusAdapter::OnConnected(void)
{
    UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessConnected() %s connect to %s:%d.", FAppName.c_str(), FMBusIP.c_str(), FMBusPort);
}
//------------------------------------------------------------------------------
void CMBusAdapter::OnDisconnected(void)
{
    UFC::BufferedLog::Printf(" CMBusAdapter::OnDisconnected()");
}
//------------------------------------------------------------------------------
}  //namespace jihsun
//------------------------------------------------------------------------------

