/* 
 * File:   CapitalExecutionConnection.h
 * Author: Zhen Fan
 *
 * Created on 2011/12/15 15:01
 */
#ifndef CAPITALEXECUTIONCONNECTION_H
#define	CAPITALEXECUTIONCONNECTION_H

#include "UFC.h"
#include "OrderConnection/TTaifexConnection.h"
#include "TandemObject.h"
#include "CDataExchangePool.h"

namespace capital
{    
//------------------------------------------------------------------------------
class CCapitalExecutionConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:
    UFC::AnsiString                  FConfigFileName;
    UFC::AnsiString                  FApplicationName;
    Int32                            FRecoverBeginTime;
    CDataExchangePool                *FDataPoolPtr;
    UFC::AnsiString                  FTaifexIP;
    Int32                            FTaifexPort;
    UFC::AnsiString                  FID;
    UFC::AnsiString                  FPassword;
    UFC::AnsiString                  FAccount;
    UFC::AnsiString                  FBrokerID;
    UFC::AnsiString                  FApKind;
    bool                             FNeedSendQuoteExecutionReport;
    UFC::PHashedSet<UFC::AnsiString> FExcludeBrokerIDSet;
    TTaifexConnection*               FTaifexConnectionPtr;
    bool                             FIsConnected;
    bool                             FIsLogon;

    void LoadSetting();
    void PrintSetting();
    void AddExecution(TExecutionReportMessage* Msg);
public:
    CCapitalExecutionConnection(const UFC::AnsiString& ApplicationName, const UFC::AnsiString& ConfigFileName, 
                                Int32 RecoverBeginTime, CDataExchangePool *DataPoolPtr = 0);
    ~CCapitalExecutionConnection();

    void Recover(const UFC::AnsiString& BeginTime);
    
    //implement interface IOrderConnectionEventListener
    virtual void OnConnected(void);
    virtual void OnDisconnected(void);
    virtual void OnLogonReply(const char* ReplyString, LogonResult Result, int CID);
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup);
    virtual void OnRecoverFinished(int Count);
    
    //implement interface UFC::PThread
    virtual void Execute( void );    
};  //CCapitalExecutionConnection
}  //namespace capital
#endif	/* CAPITALEXECUTIONCONNECTION_H */

