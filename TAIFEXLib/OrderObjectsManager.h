#ident "@(#) $Id: OrderObjectsManager.h,v 1.15 2004/05/21 10:34:21 ap27 Exp $"
#ifndef __OrderObjectsManager_H
#define __OrderObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Sigo/Sigo.h"
#include "../QueryFCM.h"
#include "../../inc/cch.h"
#include "../../tap_client/TapClientCls.h"
#include "TransactionObject.h"
#include "OrderConnectionObject.h"
#include "Utility.h"
//----------------------------------------------------------------------------------------------------------------------
using namespace UFC;
//----------------------------------------------------------------------------------------------------------------------
class TAIFEXOrderManager : public ThreadListener, public ConnectionListener
{
private:
	PEvent                  FEvent;
	PList                   FConnectionObjects;
	PCriticalSection        FCriticalSection;
	int				        FLinkNumber;
	int				        FGroupCount;
	int                     FOpenSwitch;
	OrderTransactionObject* FTransactionObject;
	OrderCheck*             FOrderCheck;
public:
	TAIFEXOrderManager( int LinkNumber, char* TAPIPFile, char* QIFIPFile );
	virtual ~TAIFEXOrderManager();
	void CreateConnectionObject( AnsiString BrokerID, AnsiString Pwd, int PVC );
	void CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName, BOOL IsSim );
	void ResetPVC( int PVC );
	int	 GetPVCState( int PVC );
	void Run();
	void OnOpenSwitch( int Value );
	void LoadTable( AnsiString DBServerName, AnsiString UserName, AnsiString  Password, AnsiString  DatabaseName, char *DBFileName );
	void ResetPCM( void );
	void GetStates( XBIO4States* States );
 	void StopConnection( int PVC );
	ConnectionObjectBase* GetConnection( int PVC );
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);	
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
