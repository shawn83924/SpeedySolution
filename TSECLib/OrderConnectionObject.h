#ident "@(#) $Id: OrderConnectionObject.h,v 1.11 2004/05/22 15:36:40 ap27 Exp $"
#ifndef __ORDER_CONNECTION_OBJECT_H
#define __ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "Messages.h"
#include "OrderObjectsManagerThread.h"
#include "LinkSubSystem.h"

#include "Session.h"
#include "fix44/NewOrderSingle.h"
#include "fix44/OrderCancelRequest.h"
#include "fix44/OrderCancelReplaceRequest.h"
#include "fix44/OrderStatusRequest.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
class TransactionObject; 
class TSECOrderManager;
class TSECOrderConnectionObject;
class OrderInfo;
//-----------------------------------------------------------------------------------------------------------------------
class TWSEConnection
{
public:    
    virtual ~TWSEConnection() {}
    virtual int  GetStatus() = 0;
    virtual void StopHandshake() = 0;
    virtual BOOL OnlineHandshake( BOOL ReceivedL010 ) = 0;
    virtual void SetLogManager( UFC::BufferedLog* LogObj ) = 0;
    virtual void Open( ConnectionParameter& Params ) = 0;            
    virtual void Close() = 0;    
    virtual int  GetSocketFD( void ) = 0;
    virtual void SetSocketFD( int ) = 0;
    virtual MessageHeader* ReceiveMessage( int Sec, BOOL ThrowReconnectException  ) = 0;
    virtual BOOL           SendMessage( MessageHeader* Msg, int Timeout ) = 0;
    virtual void  SendFIXFill( R3Body* Msg, OrderInfo* MatchOrder, BOOL Filled ) = 0;

    virtual void  OnLogon( void ) {}
    virtual void  OnLogoff( void ){}
    virtual void  onMessage( const FIX44::NewOrderSingle& ){}
    virtual void  onMessage( const FIX44::OrderCancelRequest& ){}
    virtual void  onMessage( const FIX44::OrderCancelReplaceRequest& ){}
    virtual void  onMessage( const FIX44::OrderStatusRequest& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
class TWSETCPConnection : public TWSEConnection
{
private:        
    LinkSubSystemTAIFEX* FConnection;
public:    
    TWSETCPConnection( BOOL IsTWSE, ConnectionParameter& Params )
    :FConnection( new LinkSubSystemTAIFEX( IsTWSE, Params ) ){}
    virtual int  GetStatus()                               { return FConnection->GetStatus(); }
    virtual void StopHandshake()                           { FConnection->StopHandshake(); }
    virtual BOOL OnlineHandshake( BOOL ReceivedL010 )      { return FConnection->OnlineHandshake(); }
    virtual void SetLogManager( UFC::BufferedLog* LogObj ) { FConnection->SetLogManager( LogObj ); }
    virtual void Open( ConnectionParameter& Params )       { FConnection->Open( Params ); }
    virtual void Close()                                   { FConnection->Close( ); }    
    virtual int  GetSocketFD( void )                       { return FConnection->GetSocketFD( ); }      
    virtual void SetSocketFD( int fd )                     { FConnection->SetSocketFD( fd ); }     
    virtual MessageHeader* ReceiveMessage( int Sec, BOOL ThrowReconnectException )
    { 
        return FConnection->ReceiveMessage( Sec, ThrowReconnectException ); 
    }     
    virtual BOOL SendMessage( MessageHeader* Msg, int Timeout )
    {
        return FConnection->SendMessage( Msg, Timeout );
    }
    virtual void SendFIXFill( R3Body* Msg,OrderInfo* MatchOrder, BOOL Filled ) { UFC::BufferedLog::Printf( " Not a FIX session." ); }
};
//-----------------------------------------------------------------------------------------------------------------------
class ClOrdIDInfo
{
public:    
    UFC::AnsiString   OrdClOrdID;    
    UFC::AnsiString   LastClOrdID;
};
//-----------------------------------------------------------------------------------------------------------------------
class TWSEFIXConnection : public TWSEConnection
{
private:        
    FIX::Session*              FSession;
    int                        FFD; 
    UFC::AnsiString            FSenderCompID;
    UFC::AnsiString            FTargetCompID;        
    UFC::AnsiString            FTargetSubID;    
    UFC::AnsiString            FPVCID;
    TSECOrderConnectionObject* FOwner; 
    BOOL                       FShaking;
    UFC::PtrQueue<MessageHeader> FMessages;
    UFC::PCriticalSection                       FClOrdIDLock;
    UFC::PHashMap<UFC::AnsiString,ClOrdIDInfo*> FClOrdIDMap;
private: 
    char AccountFlagTMP2FIX( char TMPFlag );    
    void NormalExecution( MessageT020* Msg );
    void OddExecution( MessageO020* Msg );
    void IntradayOddExecution( MessageO120* Msg );
    void PostExecution( MessageP020* Msg );
public:    
    TWSEFIXConnection( BOOL IsTWSE, ConnectionParameter& Params, TSECOrderConnectionObject*  Owner  );
    virtual int  GetStatus();
    virtual void StopHandshake();
    virtual BOOL OnlineHandshake( BOOL ReceivedL010 );
    virtual void SetLogManager( UFC::BufferedLog* LogObj );
    virtual void Open( ConnectionParameter& Params );
    virtual void Close();
    virtual int  GetSocketFD( void )   { return FFD; }
    virtual void SetSocketFD( int fd ) { FFD = fd; }
    virtual MessageHeader* ReceiveMessage( int Sec, BOOL ThrowReconnectException );        
    virtual BOOL SendMessage( MessageHeader* Msg, int Timeout );
    virtual void SendFIXFill( R3Body* Msg,OrderInfo* MatchOrder,  BOOL Filled );
    
    virtual void  OnLogon( void );
    virtual void  OnLogoff( void );
    virtual void  onMessage( const FIX44::NewOrderSingle& );
    virtual void  onMessage( const FIX44::OrderCancelRequest& );
    virtual void  onMessage( const FIX44::OrderCancelReplaceRequest& );
    virtual void  onMessage( const FIX44::OrderStatusRequest& );    
};
//-----------------------------------------------------------------------------------------------------------------------
//   TAIFEX Side: Connection Object for AP code 4 ( Order )
//-----------------------------------------------------------------------------------------------------------------------
class TSECOrderConnectionObject : public ConnectionObjectBase, public UFC::PThread
{
private:    
    BOOL                 FIsFIX;
private:
	BOOL                 FNeedLock;
	BOOL                 FNeedReset;
	BOOL                 FConnected;
	BOOL                 FBackup;
	TransactionObject*   FTransactionObj;
	ConnectionListener*  FConnectionListener;
    TSECOrderManager*    FOwner;
	TWSEConnection*      FConnection;
    UFC::BufferedLog*    FLog;
	void MessageLoop( void );
	void LockLoop( void );
	virtual void Execute( void );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 = FALSE );
	MessageHeader* CheckReceiveMessage( void );
public:
	TSECOrderConnectionObject(  BOOL IsTWSE,
                                ConnectionParameter& info,
                                TransactionObject*  TObj,
                                TSECOrderManager*   Owner  );
	virtual ~TSECOrderConnectionObject();
    void SetLogManager( UFC::BufferedLog* LogObj );
    void WaitForConnected( void );
    void SendMessage( MessageHeader* Msg );
    TWSEConnection* GetConnectionObject() { return FConnection; }
    TSECOrderManager* GetManager() { return FOwner; }
public:
    virtual void Active( void ) { UFC::PThread::Start(); }
    virtual BOOL Accept( int FD );
	virtual void Close( void );
	virtual void OnCommand( int Command, void* Data );
	virtual void UpdateConnectionParameter( ConnectionParameter& );
    virtual BOOL IsEnable( void ) { return TRUE; }
};
//-----------------------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
