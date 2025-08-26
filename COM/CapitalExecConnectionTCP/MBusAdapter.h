/* 
 * File:   MBusAdapter.h
 * Author: Zhen Fan
 *
 * Created on 2011/12/15 10:01
 */
#ifndef MBUSADAPTER_H
#define	MBUSADAPTER_H

#include "UFC.h"
#include "Sigo.h"

namespace capital
{
//------------------------------------------------------------------------------
class CMBusAdapter : public MonitorListener
{
private:
    UFC::AnsiString FConfigFileName;
    UFC::AnsiString FAppName;
    UFC::AnsiString FAppVersion;
    UFC::AnsiString FAppDescription;
    UFC::AnsiString FMBusIP;
    Int32           FMBusPort;
    UFC::AnsiString FAlertSubject;
    UFC::AnsiString FAlertKey;
    UFC::AnsiString FNoticeSubject;
    UFC::AnsiString FNoticeKey;
    MessageObject   *FMsgObjPtr;
    
    void LoadSetting();
    void PrintSetting();
    
public:
    CMBusAdapter(const UFC::AnsiString& ApplicationName, const UFC::AnsiString& ConfigFileName);
    virtual ~CMBusAdapter();
    
    bool IsTerminate();
    
    bool SendAlert(const UFC::AnsiString& AlertMessage);
    bool SendNotice(const UFC::AnsiString& NoticeMessage);
    
    //implement interface MonitorListener
    virtual void OnProcessStartup(const UFC::AnsiString& Host, const UFC::AnsiString& AppName);
    virtual void OnProcessStopped(const UFC::AnsiString& Host, const UFC::AnsiString& AppName);
    virtual void OnProcessConnected(BOOL IsTheFirstOne);
    virtual void OnProcessList(UFC::PStringList& Processs){};
    virtual void OnConnected(void);
    virtual void OnDisconnected(void);
};
}  //namespace capital
#endif	/* MBUSADAPTER_H */
