//------------------------------------------------------------------------------------------------------------------------
#ifndef __RCQGConnection_H
#define __RCQGConnection_H
//------------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFCType.h"
#include "../UFC/PClientSocket.h"
#include "../UFC/MemoryStream.h"
#include "RCQGProtocol.h"
#include "RCQGNameValueSet.h"
//------------------------------------------------------------------------------------------------------------------------
class RCQGConnection;
//------------------------------------------------------------------------------------------------------------------------
// class RCQGConnectionEventListener
//------------------------------------------------------------------------------------------------------------------------
class RCQGConnectionEventListener
{
public:
	virtual void OnDisconnect( RCQGConnection* Connection ) = 0;
	virtual void OnDataArrived( RCQGConnection* Connection, RCQGNameValueSet* Data ) = 0;
public:
	virtual ~RCQGConnectionEventListener() {}
};
//------------------------------------------------------------------------------------------------------------------------
// class RCQGConnection
//------------------------------------------------------------------------------------------------------------------------
class RCQGConnection : public UFC::SocketClientListener
{
private:
	UFC::PClientSocket*				FClientSocket;
	RCQGConnectionEventListener*	FListener;
private:
	UFC::PCriticalSection			FSyncCS;
	BOOL							FSyncMode;   ///< Flag Is Sync Mode
	BOOL							FSyncResult; ///< Flag Is Result succeed
	UFC::PEvent						FSyncEvent;
	RCQGNameValueSet				FSyncNameValue;
private: ///< SocketClientListener Implementation
	void	OnConnect( UFC::PClientSocket* Socket );
	void	OnDisconnect( UFC::PClientSocket* Socket, BOOL NeedReconnect = FALSE );
	BOOL	OnDataArrived( UFC::PClientSocket* Socket );
	void	OnIdle( UFC::PClientSocket* Socket );
private:
	void	LockAndSend( RCQGPacket& PacketData );
	void	ClearSyncData( void );
	void	SetSyncData( void );
	void	AckErrSyncData( void );
	void	ProcQueryResult( RCQGNameValueSet* Data );
public:
	int		Connect( char* IP, int Port, int TimeoutSec, RCQGNameValueSet& LogonConfirm );
	BOOL	Disconnect();
	BOOL	IsConnected();
	int		SendNameValue( RCQGNameValueSet& Data );
	int		SyncSendNameValue( RCQGNameValueSet& Data, RCQGNameValueSet& ResultData, int TimeOutSec );
public:
	RCQGConnection( RCQGConnectionEventListener* Listener );
	~RCQGConnection();
};
//------------------------------------------------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------------------------------------------------
