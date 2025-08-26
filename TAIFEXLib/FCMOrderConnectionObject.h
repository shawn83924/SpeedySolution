#ifndef __FCM_ORDER_CONNECTION_OBJECT_H
#define __FCM_ORDER_CONNECTION_OBJECT_H
//-----------------------------------------------------------------------------------------------------------------------
#include "ConnectionObject.h"
class FCMOrderClient;
class OrderGenerator;
class ConnectionListener;
//-----------------------------------------------------------------------------------------------------------------------
//   FCM Side: Connection Object for AP code 4
//-----------------------------------------------------------------------------------------------------------------------
class FCMOrderConnectionObject : public ConnectionObjectBase,public PThread
{
private:
	FCMOrderClient*	     FOrderFCM;
	OrderGenerator*      FOrderIssuer;
	ConnectionListener*  FConnectionListener;
	int					 FOrderNumber;
	ConnectionParameter  FParams;
private:
    void SendReport( void );
	virtual void Execute( void );
public:
   FCMOrderConnectionObject( ConnectionParameter& info,
                             ConnectionListener* CListener,
                             ThreadListener* TListener );
	virtual ~FCMOrderConnectionObject();
public:
	virtual void Close( void ){}
	virtual void OnCommand( int , void*  ){}
	virtual void UpdateConnectionParameter( ConnectionParameter& ){}
};
//-----------------------------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------------------------
