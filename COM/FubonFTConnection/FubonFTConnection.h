// 
// File:   FubonOrderConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _FubonOrderConnection_H
#define	_FubonOrderConnection_H
//------------------------------------------------------------------------------
#include "Sysque.h"
#include "../../UFC/UFC.h"
#include "../../Migo/Sigo.h"
//--------------------------------------------------------------------------
class TFubonOrderConnection : public MessageListener, public UFC::PThread
{
private:    
    UFC::AnsiString       FDebug;
    UFC::AnsiString       FLogFile;
    UFC::AnsiString       FLockFile;
    UFC::AnsiString       FInbox;
    UFC::AnsiString       FIP;
    UFC::AnsiString       FHostname;
    int                   FPort;
    UFC::AnsiString       FQueIP;
    int                   FQuePort;
    UFC::AnsiString       FReqQueName;
    int                   FReadBufferSize;     
    TSysque*              FReqQueue;        
    char*                 FReadBuffer;
    UFC::PCriticalSection FConnectionStateCS;
    UFC::PHashedList<UFC::AnsiString, TSysque*> FBrokerTable;    
    UFC::PHashedSet<UFC::AnsiString>            FBroadcastSet;    
private:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );    
public:
    virtual void Execute( void );    
public:
    TFubonOrderConnection( const UFC::AnsiString& ConfigFileName );
    virtual ~TFubonOrderConnection( void );
private:    
    BOOL SaveFile( MTree* Data, const UFC::AnsiString& FileCode, UFC::AnsiString& FileName );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void OpenQueues( const UFC::AnsiString& ConfigFileName );
    void LockFile( void );
    void Dequeue( void );
    void OnData( void );
    void FileRequest( TSQMessage& SQMessage );
    void FileUpload( TSQMessage& SQMessage );   
    //void RejectRequest( TSQMessage& Req );
    void ToSysQueue(  const UFC::AnsiString& BrokerID,  TSQMessage& ReplyMessage );
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
