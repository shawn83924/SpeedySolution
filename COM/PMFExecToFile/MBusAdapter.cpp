/* 
 * File:   MBusAdapter.h
 * Author: Zhen Fan
 *
 * Created on 2016/08/19 08:28
 */

#include "GlobalVariable.h"
#include "MBusAdapter.h"

//-------------------------------- CMBusAdapter --------------------------------
//------------------------------------------------------------------------------
CMBusAdapter::CMBusAdapter()
:FMBusConnectionPtr(0)
{
    FMBusConnectionPtr = new MessageObject(gAppName, gAppVersion, gAppDescription, gMBusPort);
    FMBusConnectionPtr->SetHost(gMBusIP, gMBusPort);
    FMBusConnectionPtr->SetMonitorListener(this);    
//    FMBusConnectionPtr->AddListener(FClientOrderSubject, FClientOrderKey, this);
    FMBusConnectionPtr->Start();
    FMBusConnectionPtr->WaitForConnected();
    UFC::BufferedLog::Printf(" After Wait For MBus Connected.");
}  //CMBusAdapter::CMBusAdapter()
//------------------------------------------------------------------------------
CMBusAdapter::~CMBusAdapter()
{
    if (FMBusConnectionPtr != 0) delete FMBusConnectionPtr;
}  //CMBusAdapter::~CMBusAdapter()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessStartup(const UFC::AnsiString& Host, const UFC::AnsiString& AppName)
{   //implement interface MonitorListener
    UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessStartup() Process[%s] Start at Host[%s]", AppName.c_str(), Host.c_str());
}  //CMBusAdapter::OnProcessStartup()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessStopped(const UFC::AnsiString& Host, const UFC::AnsiString& AppName)
{   //implement interface MonitorListener
    UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessStartup() Host[%s] Process[%s] Stopped. ", Host.c_str(), AppName.c_str());
}  //CMBusAdapter::OnProcessStopped()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessConnected(BOOL IsTheFirstOne)
{   //implement interface MonitorListener
    if (IsTheFirstOne)
        UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessConnected() is the First Application.");
    else
    {
        UFC::BufferedLog::Printf(" CMBusAdapter::OnProcessConnected() Same Application is Connected, Exit.");
        exit(-1);
    }
}  //CMBusAdapter::OnProcessConnected()
//------------------------------------------------------------------------------
void CMBusAdapter::OnProcessList(UFC::PStringList& Processs)
{   //implement interface MonitorListener
}  //CMBusAdapter::OnProcessList())
//------------------------------------------------------------------------------
void CMBusAdapter::OnConnected(void)
{   //implement interface MonitorListener
    UFC::BufferedLog::Printf(" CMBusAdapter::OnConnected()");
}  //CMBusAdapter::OnConnected()
//------------------------------------------------------------------------------
void CMBusAdapter::OnDisconnected(void)
{   //implement interface MonitorListener
    UFC::BufferedLog::Printf(" CMBusAdapter::OnDisconnected()");
}  //CMBusAdapter::OnDisconnected()
//------------------------------------------------------------------------------
void CMBusAdapter::OnMigoMessage(const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data)
{   //implement interface MessageListener
}  //CMBusAdapter::OnMigoMessage()



