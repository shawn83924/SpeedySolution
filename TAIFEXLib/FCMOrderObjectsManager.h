#ident "@(#) $Id: FCMOrderObjectsManager.h,v 1.7 2004/05/21 10:34:20 ap27 Exp $"
#ifndef __FCM_ORDER_Objects_Manager_H
#define __FCM_ORDER_Objects_Manager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../QueryFCM.h"
#include "FCMOrderConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class FCMOrderManager : public ThreadListener, public ConnectionListener
{
private:
	PList              FConnectionObjects;
	int                FFCMCount;
	int				   FTotalOrder;
	long			   FTotalTime;
	int				   FLinkNumber;
	int                FIssuerType;
	int				   FInteger;
	int                FMaxTime;
	char*              FString;
public:
	FCMOrderManager( int LinkNumber, int IssuerType, char* StringData = NULL, int IntData = 0);
	virtual ~FCMOrderManager();
	void CreateConnectionObjects( int MaxPVC = 0 );
	void Run();
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
