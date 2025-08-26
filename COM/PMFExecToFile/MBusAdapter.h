/* 
 * File:   MBusAdapter.h
 * Author: Zhen Fan
 *
 * Created on 2016/08/19 08:28
 */
#include "UFC.h"
#include "Sigo.h"

#ifndef MBUSADAPTER_H
#define MBUSADAPTER_H
//------------------------------------------------------------------------------
class CMBusAdapter : public MessageListener, public MonitorListener
{
public:    
    MessageObject* FMBusConnectionPtr;

public:
    CMBusAdapter();
    ~CMBusAdapter();
    
    //implement interface MonitorListener
    virtual void OnProcessStartup(const UFC::AnsiString& Host, const UFC::AnsiString& AppName);
    virtual void OnProcessStopped(const UFC::AnsiString& Host, const UFC::AnsiString& AppName);
    virtual void OnProcessConnected(BOOL IsTheFirstOne);
    virtual void OnProcessList(UFC::PStringList& Processs);
    virtual void OnConnected(void);
    virtual void OnDisconnected(void);
    
    //implement interface MessageListener
    void OnMigoMessage(const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree *Data);
    
};  //CMBusAdapter


#endif /* MBUSADAPTER_H */

