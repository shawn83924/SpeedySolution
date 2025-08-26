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
#include "../../UFC/UFC.h"
#include "../../Migo/Sigo.h"
//--------------------------------------------------------------------------
class TMegaFTConnection : public MessageListener, public UFC::PThread
{
private: ///< For Program   
    UFC::AnsiString       FDebug;
    UFC::AnsiString       FLogFile;
    UFC::AnsiString       FInbox;
private: ///< For MBus    
    UFC::AnsiString       FIP;
    UFC::AnsiString       FHostname;
    int                   FPort;
private: ///< For Request Queue    
    key_t                 FReqQueueID;
    UFC::PQueue           FReqQueue;        
    key_t                 FRespQueueID;
    UFC::PQueue           FRespQueue;        
private: ///< Response Queues   
    UFC::PCriticalSection                           FConnectionStateCS;
    UFC::PHashedList<UFC::AnsiString, UFC::PQueue*> FBrokerTable;    
    UFC::PHashedSet<UFC::AnsiString>                FBroadcastSet;    
private:
    void FLEXResponse( char Market, const UFC::AnsiString& Subject, MTree* Data );
    void TAIFEXFTResponse( char Market, const UFC::AnsiString& Subject, MTree* Data );
    void TWSEFTResponse( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
private:    
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key,  MTree* Data );    
public:
    virtual void Execute( void );    
public:
    TMegaFTConnection( const UFC::AnsiString& ConfigFileName );
    virtual ~TMegaFTConnection( void );
private:    
    BOOL SaveFile( MTree* Data, const UFC::AnsiString& FileCode, UFC::AnsiString& FileName );
    void SetLogObject( void );
    void ReadConfigFile( const UFC::AnsiString& ConfigFileName );
    void OpenQueues( const UFC::AnsiString& ConfigFileName );
    void Dequeue( void );
    void OnData( char* Beffer );
    void OnTWSEOTCData( char* Buffer );
    void OnTAIFEXFTData( char* Buffer );
    void OnTAIFEXFLEXData( bool IsNew, char* Buffer );
    void FileUpload( const UFC::AnsiString&  BrokerID,
		     const UFC::AnsiString&  FileCode,
		     const UFC::AnsiString&  RequestID,
		     const UFC::AnsiString&  FileName,
		     char Market );
    void FileRequest( const UFC::AnsiString&  BrokerID,
		      const UFC::AnsiString&  FileCode,
		      const UFC::AnsiString&  RequestID,
		      const UFC::AnsiString&  ReqData,
		      char Market );
    void ToQueue(  const UFC::AnsiString& BrokerID,  const UFC::AnsiString& ReplyMessage );
};
//------------------------------------------------------------------------------
#endif	/* _FubonOrderConnectionEventListener_H */
