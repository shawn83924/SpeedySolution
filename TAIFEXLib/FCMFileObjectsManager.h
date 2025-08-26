#ident "@(#) $Id: FCMFileObjectsManager.h,v 1.3 2004/05/21 10:34:20 ap27 Exp $"
#ifndef __FCM_FILE_OBJECT_MANAGER_H
#define __FCM_FILE_OBJECT_MANAGER_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../QueryFCM.h"
#include "FCMFileConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class FCMFileObjectsManager : public ThreadListener, public ConnectionListener
{
private:
	int                FFCMCount;
	int				   FCMCount;
	PEvent             FEvent;
	PList              FConnectionObjects;
	int				   FLinkNumber;
	char*			   FIPAddress;
	int                FPort;
public:
	FCMFileObjectsManager( int LinkNumber, char* IP = NULL, int Port = 0 );
	virtual ~FCMFileObjectsManager();
	void CreateConnectionObjects( );
	void Run();
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
