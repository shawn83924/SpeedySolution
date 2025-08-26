// 
// File:   YuantaExecConnection.h
// Author: yuan
//
// Created on September 4, 2007, 4:24 PM
//
//------------------------------------------------------------------------------
#ifndef _SinoPacExecConnection_H
#define	_SinoPacExecConnection_H
//------------------------------------------------------------------------------
#include "../Lib/OrderConnection/TTaifexConnection.h"
#include "../OrderConnection/Resource.h"
#include "tibrv/tibrvcpp.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class MStreamWriter
{
private:
	char* FPrt;
	int   FPos;
public:
	MStreamWriter( char* Memory ):FPrt(Memory),FPos(0){}
	void Write( const char* Data, int Len )
	{
		memcpy( FPrt + FPos,Data, Len);
		FPos += Len;
	}
	void AppendZero( void )
	{
		AppendChar( 0 );
	}
	void AppendChar( char ch )
	{
		*( FPrt + FPos ) = ch;
		FPos ++;
	}
	int GetPosition(){ return FPos; }
};
//--------------------------------------------------------------------------
class SinoPacExecConnection : public IOrderConnectionEventListener, 
                              public UFC::PThread,
                              public TibrvMsgCallback
{
private:    
    UFC::AnsiString       FDebug;
    UFC::AnsiString       FAppName;
    UFC::AnsiString       FIP;
    int                   FPort;
    UFC::AnsiString       FID;
    UFC::AnsiString       FPassword;   
    UFC::AnsiString       FAccount;   
    UFC::AnsiString       FBrokerID;       
    UFC::AnsiString       FLineNo;   
private:    
    UFC::AnsiString       FService;           
    UFC::AnsiString       FNetwork;           
    UFC::AnsiString       FDaemon;               
private:
    int                   FRecoverBeginTime;    
    TTaifexConnection*    FConnection; 
    BOOL                  FIsLogon;       
private:    
    UFC::AnsiString       FOrderSubject;
    UFC::AnsiString       FConfirmSubject;   
    UFC::AnsiString       FConfirmPrefixDef;       
    UFC::AnsiString       FConfirmTrackingID;       
    UFC::AnsiString       FConfirmTracking1;       
    UFC::AnsiString       FFillSubject;       
    UFC::AnsiString       FFillPrefixDef; 
    UFC::AnsiString       FFillTrackingID;
    UFC::AnsiString       FFillTracking1;    
private:        
    TibrvNetTransport     FRVTransport;     
    TibrvListener         FOrderlistener;
    TibrvMsg              FConfirmMsg;
    TibrvMsg              FConfirmDataMsg;
    TibrvMsg              FConfirmPrefixMsg;
    TibrvMsg              FConfirmTrackingMsg;
    TibrvMsg              FFillMsg;
    TibrvMsg              FFillDataMsg;
    TibrvMsg              FFillPrefixMsg;
    TibrvMsg              FFillTrackingMsg;
private:    
    void InitRV( void );
    void CreateRVMsg( void );
    void LoadSetting( void );    
    void ConfirmToBackend( TExecutionReportMessage* Msg );
    void FillToBackend( TExecutionReportMessage* Msg );
    void FillToCancel( TExecutionReportMessage* Msg );
private: ///< Implement IOrderConnectionEventListener
    virtual void OnConnected( void );
    virtual void OnDisconnected( void );
    virtual void OnLogonReply( const char* ReplyString, LogonResult Result, int CID );
    virtual void OnExecutionReport( TExecutionReportMessage* Msg, ExecDup PossDup );
    virtual void OnRecoverFinished( int Count );
private: ///< Implement PThread    
    virtual void Execute( void );    
private: ///< Implement TibrvMsgCallback    
    void onMsg(TibrvListener* listener, TibrvMsg& msg );
private:    
    void NewOrder(  UFC::AnsiString& Order );
    void CancelOrder(  UFC::AnsiString& Order );
    void ReduceOrder(  UFC::AnsiString& Order, BOOL IsReplacePX );
public:
    SinoPacExecConnection( int RecoverBeginTime );
    virtual ~SinoPacExecConnection( void );   
    void CheckConnection( void );
    void Recover( UFC::AnsiString BeginTime );    
};
//------------------------------------------------------------------------------
#endif
