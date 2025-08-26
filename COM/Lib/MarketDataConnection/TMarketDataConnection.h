//---------------------------------------------------------------------------

#ifndef TMarketDataConnectionH
#define TMarketDataConnectionH
//---------------------------------------------------------------------------
#include "../MessageDeliver/TMdMessage.h"
#include "../MessageDeliver/TMdTransport.h"
#include "../MessageDeliver/TMdListener.h"

#include "TMarketDataMessage.h"
#include "TMDQuoteRequestMessage.h"
#include "TSecurityDefinitionRequest.h"
#include "TSecurityDefinition.h"
#include "TNewsMessage.h"
//---------------------------------------------------------------------------
class TMarketDataConnectionEventListener
{
public:
	virtual void OnConnected( void ) = 0;
	virtual void OnDisconnected( void ) = 0;
	virtual void OnSecurityDefinition( TSecurityDefinitionMessage* Msg ) = 0;
	virtual void OnNews( TNewsMessage* Msg ) = 0;
	virtual void OnMarketDataMessage( TMarketDataMessage* Msg ) = 0;
	virtual void OnQuoteRequest( TMDQuoteRequestMessage* Msg ) = 0;
};
//---------------------------------------------------------------------------
class TMarketDataConnection : public MAppListener, public MessageListener
{
private:
	HINSTANCE							FInstance;
	TMarketDataConnectionEventListener*	FListener;
	TMdTransport*						FTransport;
	UFC::AnsiString						FAppName;
	UFC::PCriticalSection				FCS;
	UFC::PHashedList<UFC::AnsiString, TMdListener*>		FRegister;
	TMdListener*						FQRListener;
	TMdListener*						FSDListener;
	TMdListener*						FNewsListener;
private:
	void AddToRegister( const char* Key, TMdListener* Listener );
	void RemoveFromRegister( const char* Key );
	void ReceivedMarketData( MTree* pTree );
	void ReceivedQuoteRequest( MTree* pTree );
	void ReceivedSecurityDefinition( MTree* pTree );
	void ReceivedNews( MTree* pTree );
public:
	virtual void						OnMAppConnected( void );
	virtual void						OnMAppDisconnected( void );
	virtual void						OnMAppError( PMAppError Error );
public:
	virtual void						OnMigoMessage( UFC::AnsiString Subject, UFC::AnsiString Key,  MTree* pTree );

public:
	TMarketDataConnection( HINSTANCE AppInstance, const char* AppName, TMarketDataConnectionEventListener* TMarketDataConnectionEventListener );
	~TMarketDataConnection( void );
public:
	void								Connect( const char* RemoteIP, long RemotePort );
	void								Disconnect( void );
	void								Subscribe( const char* Market, const char* SecurityID );
	void								UnSubscribe( const char* Market, const char* SecurityID );
	void								SecurityDefinitionRequest( TSecurityDefinitionRequestMessage* Msg );
};
#endif
