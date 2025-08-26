#ifndef __TMPOrderObjectsManager_H
#define __TMPOrderObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../ExchangeUtility/Utility.h"
#include "../TMP89/QuerySessions.h"
#include "TMPExecConnectionObject.h"
#include "../../inc/odm.h"
#include "../../inc/TMP_stats.h"
#include "../TMP89/MexObject.h"
//----------------------------------------------------------------------------------------------------------------------
class TMPExecManager : public UFC::ThreadListener, public ConnectionListener
{
private:
	UFC::PEvent             FEvent;
	UFC::PList              FConnectionObjects;
	UFC::PCriticalSection   FCriticalSection;
	int	                FListenPort;
        int                     FConnectedCount;
        LogManager*             FLog;        
private:
	UFC::PCriticalSection                            FSessionTableCS;
        UFC::PHashedList<UInt32, ConnectionObjectBase*>  FSessionsTable;
        UFC::List<ConnectionObjectBase*>                 FSessionsArray;        
public:
	TMPExecManager( int LinkNumber, const UFC::AnsiString& OCFDate );
	virtual ~TMPExecManager();
	void CreateConnectionObjects( TMPSessions* Sessions );
	void ResetPVC( int BrokerIndex,int PVC );
	int  GetPVCState( int BrokerIndex,int SID, Int32& Connected  );
	void Run();
 	void StopConnection( int BrokerIndex,int PVC );	
	// Implement ThreadListener interface.
	virtual void OnTerminate( UFC::PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);	
public:        
        ConnectionObjectBase* GetSession(  UInt32 FCMID, UInt16 SID  );
        ConnectionObjectBase* GetSession(  UInt32 SID  );
public:
        UInt16 GetProcessID( void ) { return FListenPort%1000; }
        void GetStates( TMP89States* States );
};        
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
