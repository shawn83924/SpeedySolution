// 
// File:   WLSExecConnection.h
// Author: Simon Chang
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _WLSExecConnection_H
#define	_WLSExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include"../UFC/PInt32.h"
//--------------------------------------------------------------------------
const UInt32 WLS_ORDER_LEN = 86;
const UInt32 WLS_CONFIRM_LEN = 116;
const UInt32 WLS_FILL_LEN = 123;
//--------------------------------------------------------------------------
class WLSExecConnection : public IOrderConnectionEventListener, public UFC::PThread
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
private:    
    UFC::AnsiString    FOrderPath;
    UFC::AnsiString    FConfirmPath;
    UFC::AnsiString    FFillPath;
    UFC::AnsiString    FLogPath;
private:
    int                   FRecoverBeginTime;    
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;        
    UFC::FileStreamEx*    FOrderStream;
    UFC::FileStreamEx*    FConfirmStream;    
    UFC::FileStreamEx*    FFillStream;    
    UFC::PInt32           FReadPos;
    char                  FMessage[ 128 ];
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, bool IsAccepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void OpenFileStream( void );
    BOOL ReadOrder( void );
    void SendOrder( void );
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    WLSExecConnection( int RecoverBeginTime );
    virtual ~WLSExecConnection( void );   
    void Recover( UFC::AnsiString BeginTime );    
};
//------------------------------------------------------------------------------
#endif
