#ident "@(#) $Id: OrderConnectionObject.h,v 1.11 2004/05/22 15:36:40 ap27 Exp $"
#ifndef __ORDER_CONNECTION_OBJECT_H
#define __ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
#include "TransactionObject.h"
//-----------------------------------------------------------------------------------------------------------------------
class LinkSubSystemTAIFEX;
//-----------------------------------------------------------------------------------------------------------------------
//   TAIFEX Side: Connection Object for AP code 4 ( Order )
//-----------------------------------------------------------------------------------------------------------------------
class TAIFEXOrderConnectionObject : public ConnectionObjectBase, public PThread
{
private:
	BOOL                 FNeedLock;
	BOOL                 FNeedReset;
	BOOL                 FConnected;
	BOOL                 FBackup;
	TransactionObject*   FTransactionObj;
	ConnectionListener*  FConnectionListener;
	LinkSubSystemTAIFEX* FConnection;
    LogManager*          FLog;
	void MessageLoop( void );
	void LockLoop( void );
	virtual void Execute( void );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 = FALSE );
	MessageHeader* CheckReceiveMessage( void );
public:
	TAIFEXOrderConnectionObject(  ConnectionParameter& info,
				                  TransactionObject*   TObj,
				                  ConnectionListener*  Listener,
				                  ThreadListener*      TListener );
	virtual ~TAIFEXOrderConnectionObject();
    void SetLogManager( LogManager* LogObj );
public:
	virtual void Close( void );
        virtual void Active( void ) { PThread::Start(); }
	virtual void OnCommand( int Command, void* Data );
	virtual void UpdateConnectionParameter( ConnectionParameter& );
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
