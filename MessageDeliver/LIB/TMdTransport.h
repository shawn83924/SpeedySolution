#ifndef _TMdTransport_H_
#define _TMdTransport_H_
//------------------------------------------------------------------------------
#include "TThreadApp.h"
#include "TMdMessage.h"
//------------------------------------------------------------------------------
/*class TTransport
{
public:
	virtual void        Connect( const char* RemoteIP, long RemotePort, int TimeoutSec ) = 0;
	virtual void        Disconnect( void ) = 0;
	virtual BOOL        IsConnected( void ) = 0;
	virtual BOOL        Send( const char* Subject, const char* Key, TMdMessage* Msg ) = 0;
	virtual BOOL        Send( UFC::MemoryStream* WriteStream ) = 0;
	virtual void        Subscribe( MSubscriber* Subscribe ) = 0;
	virtual void        Unsubscribe( MSubscriber* Subscribe ) = 0;
	virtual UFC::Int32  GetQueuesize( void ) = 0;
	virtual             ~TTransport(){};
	static  TTransport* CreateTransport(  HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL IsWin32GUIApp );
}; */
//------------------------------------------------------------------------------
class TTransport
{
private:
	TThreadApp*   FThread;
	MApp*         FApp;
public:
	TTransport( HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL  IsWin32GUIApp );
	~TTransport( void );
public:
	void            Connect( const char* RemoteIP, long RemotePort, int TimeoutSec = 10 );
	void            Disconnect( void );
	BOOL            IsConnected( void ) { return FApp->IsConnected(); }
	BOOL            Send( const char* Subject, const char* Key, TMdMessage* Msg );
	BOOL 		Send( UFC::MemoryStream* WriteStream );
	void            Subscribe( MSubscriber* Subscribe );
	void            Unsubscribe( MSubscriber* Subscribe );
	UFC::Int32      GetQueuesize( void ){ return FApp->GetQueueSize(); }
	MApp*           GetMApp() { return FApp; }
public:
	static  TTransport* CreateTransport(  HINSTANCE Instance, MAppListener* pListener, const char* AppName, BOOL IsWin32GUIApp );
};
//------------------------------------------------------------------------------
#endif
