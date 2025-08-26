#ident "@(#) $Id: FCMOrderObjectsManager.h,v 1.7 2004/05/21 10:34:20 ap27 Exp $"
#ifndef __FCM_ORDER_Objects_Manager_H
#define __FCM_ORDER_Objects_Manager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../QueryFCM.h"
#include "../Migo/Sigo.h"
#include "Utility.h"
#include "FCMOrderConnectionObject.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class OrderObj
{
public:
    UFC::AnsiString KeyString;
    UFC::AnsiString OrderString;
public:
    OrderObj( const char* Key, const char* Order )
    :KeyString( Key ),OrderString( Order ){}
};
//--------------------------------------------------------------------------------------------------------------------
class FCMObjectsManager : public ThreadListener, public ConnectionListener, public MessageListener
{
private:
	PList               FConnectionObjects;
	int                 FFCMOrderPVCCount;
    int                 FFCMConfirmPVCCount;
    int                 FCMConfirmPVCCount;
    int                 FCMOrderConfirmPVCCount;
	int				    FTotalOrder;
	long			    FTotalTime;
	int				    FLinkNumber;
	int                 FMaxTime;
    PtrQueue<OrderObj>* FOrderQueue;
public:
	FCMObjectsManager( int LinkNumber );
	virtual ~FCMObjectsManager();
	void CreateConnectionObjects( int MaxPVC = 0 );
    PtrQueue<OrderObj>* GetOrderQueur(){ return FOrderQueue; }
	void Run();
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
    // Interface MessageListener
    virtual void OnMigoMessage( AnsiString Subject, AnsiString Key,  MTree* );
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
