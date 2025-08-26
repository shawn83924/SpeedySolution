// 
// File:   SystexOrderConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SystexOrderConnection_H
#define	_SystexOrderConnection_H
//------------------------------------------------------------------------------
#include "../../UFC/UFC.h"
#include "../../Migo/Sigo.h"
#include "SQMessage.h"
//--------------------------------------------------------------------------
class TSystexFTConnection : public MessageListener
{
private:    
    UFC::AnsiString       FDebug;
    UFC::AnsiString       FLogFile;
    UFC::AnsiString       FTSEInbox;
    UFC::AnsiString       FTSEOutbox;
    UFC::AnsiString       FOTCInbox;
    UFC::AnsiString       FOTCOutbox;
    UFC::AnsiString       FFUTInbox;
    UFC::AnsiString       FOPTInbox;
    UFC::AnsiString       FIP;
    int                   FPort;        
private:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );
    void HandleTSEMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data, BOOL IsTSE, BOOL IsResponse  );
    void HandleTFXMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* Data, BOOL IsTSE, BOOL IsResponse  );
public:
    void Run( void );    
public:
    TSystexFTConnection( const UFC::AnsiString& ConfigFileName );
    virtual ~TSystexFTConnection( void );
private:    
    BOOL SaveTSEFile( bool IsTSE, MTree* Data, const UFC::AnsiString& BrokerID,const UFC::AnsiString& FileCode, UFC::AnsiString& FileName );
    BOOL SaveTFXFile( bool IsOPT, MTree* Data, const UFC::AnsiString& BrokerID,const UFC::AnsiString& FileCode, UFC::AnsiString& FileName );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void AddListeners( const UFC::AnsiString& ConfigFileName );
    void TSEFileRequest( TSQMessage& SQMessage );
    void TFXFileRequest( TSQMessage& SQMessage );
    void FileUpload( bool IsTSE, TSQMessage& SQMessage );   
    //void RejectRequest( TSQMessage& Req );
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
