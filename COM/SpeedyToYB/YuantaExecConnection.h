// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _YuantaExecConnection_H
#define	_YuantaExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
//------------------------------------------------------------------------------
const Int32 YUANTA_DATA_MAX = 256;
const long YUANTA_FUT_QUEUE = 0x72A50000;    
const long YUANTA_OPT_QUEUE = 0x73A50000;
const long YUANTA_CHANTER_REPORT_QUEUE = 0x72AE0000;
//------------------------------------------------------------------------------
class YuantaExecConnection : public IOrderConnectionEventListener, public UFC::PThread
{
private:    
    UFC::AnsiString    FDebug;
    UFC::AnsiString    FAppName;
    UFC::AnsiString    FIP;
    int                FPort;
    UFC::AnsiString    FID;
    UFC::AnsiString    FPassword;   
    UFC::AnsiString    FAccount;   
    UFC::AnsiString    FBrokerID;       
    UFC::AnsiString    FIB;       
    UFC::AnsiString    FChannelCode;       
    BOOL               FIsLogon;        
private:    
    BOOL               FToBackend;
    BOOL               FToChanter;
    BOOL               FFillToChanterOnly;
private:    
    TTaifexConnection*    FConnection; 
    UFC::PQueue           FFUTExecutionQueue;
    UFC::PQueue           FOPTExecutionQueue;
    UFC::PQueue           FChanterReportQueue;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count ) {}
public: ///< Implement PThread
    void SplashScreen( void );
    void LoadSetting( void );
    void GenerateR020( int Func, TExecutionReportMessage* Msg, char* DataBuffer );
    void ToBackend( TExecutionReportMessage* Msg );
    virtual void Execute( void );    
public:
    YuantaExecConnection( void );
    virtual ~YuantaExecConnection( void );   
};
//------------------------------------------------------------------------------
#endif
