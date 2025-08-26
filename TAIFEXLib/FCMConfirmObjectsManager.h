#ident "@(#) $Id: FCMConfirmObjectsManager.h,v 1.9 2004/05/21 10:34:20 ap27 Exp $"
#ifndef __FCM_CONFIRM_OBJECT_MANAGER_H
#define __FCM_CONFIRM_OBJECT_MANAGER_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../QueryFCM.h"
#include "FCMConfirmConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class FCMConfirmManager : public ThreadListener, public ConnectionListener
{
private:
	int                FFCMCount;
	int				   FCMCount;
	int                FOrderCMCount;
	PEvent             FEvent;
	PList              FConnectionObjects;
	int				   FLinkNumber;
public:
	FCMConfirmManager( int LinkNumber );
	virtual ~FCMConfirmManager();
	void CreateConnectionObjects( void );
	void Run();
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* ){}
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
