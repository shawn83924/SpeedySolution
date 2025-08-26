#ident "@(#) $Id: FCMConfirmConnectionObject.h,v 1.14 2004/05/26 04:30:30 ap27 Exp $"
#ifndef __FCM_CONFIRM_CONNECTION_OBJECT_H
#define __FCM_CONFIRM_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
class LinkSubSystemFCM; 
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 3
//-----------------------------------------------------------------------------------------------------------------------
class FCMConfirmConnectionObject : public ConnectionObjectBase, /*public SocketClientListener,*/ public PThread
{
private:
	BOOL                 FNeedLock;
	BOOL                 FHandshakeReady;
	int					 FSeqNumber;
	int                  F030Count;
	int                  F031Count;
	int                  F032Count;
	char                 FAPCode;
	BOOL				 FBroken;
	LinkSubSystemFCM*    FConnection;
	ConnectionListener*  FConnectionListener;
private:
	void SendSocketMessage( BOOL IsSend, MessageHeader* Msg );
private:
	// Implement interface PThread
	virtual void Execute( void );
private:
	void MessageLoop( void );
	void FCMHandshake( void );
	void CMHandshake( void );
	void ReceiveC030( MessageC030* C030 );
	void ReceiveC031( MessageC031* C031 );
	void ReceiveC032( MessageC032* C032 );
	void ConfirmsubSystemHandshake( BOOL ReceivedL010 );
public:
	FCMConfirmConnectionObject( ConnectionParameter& info,
		                        ConnectionListener*  Listener,
		                        ThreadListener*      TListener,
		                        char				 APCode,
		                        char*                IPAddress = NULL,
		                        int					 Port = 0 );
	virtual ~FCMConfirmConnectionObject();
public:
	virtual void Close( void ){}
	virtual void OnCommand( int , void* ){}
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------

