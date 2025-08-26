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
#include "../../UFC/FileStream.h"
#include "../../UFC/AnsiString.h"
//--------------------------------------------------------------------------
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
private:    
    int                   FRecoverBeginTime;    
    TResourceList         FResource;
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;        
    UFC::FileStreamEx*    FFUTConfirmFile;
    UFC::FileStreamEx*    FOPTConfirmFile;
    UFC::FileStreamEx*    FFUTFillFile;
    UFC::FileStreamEx*    FOPTFillFile;
    UFC::AnsiString     FFUTCFileName;       
    UFC::AnsiString     FFUTFFileName;       
    UFC::AnsiString     FOPTCFileName;       
    UFC::AnsiString     FOPTFFileName;
public: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, bool IsAccepted, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, bool PossDup );
    virtual void OnRecoverFinished( int Count );
public: ///< Implement PThread
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void LoadSetting( void );
    virtual void Execute( void );    
public:
    YuantaExecConnection( int RecoverBeginTime );
    virtual ~YuantaExecConnection( void );   
    void Recover( UFC::AnsiString BeginTime );    
};
//------------------------------------------------------------------------------
#endif
