#ifndef __OrderObjectsManager_H
#define __OrderObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../SimTSEC/QueryPVC.h"
#include "../TSECLib/ExchangeConst.h"
#include "OrderConnectionObject.h"
#include "Utility.h"
#include "TransactionObject.h"
#include "OrderFIXManager.h"
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//----------------------------------------------------------------------------------------------------------------------
using namespace UFC;
//----------------------------------------------------------------------------------------------------------------------
class TSECOrderManager : public MessageListener, public ThreadListener, public ConnectionListener, public PThread
{
private:
	PEvent                  FEvent;
	PList                   FConnectionObjects;
	PCriticalSection        FCriticalSection;
    int                     FLinkNumber;
	int                     FGroupCount;
	int                     FOpenSwitch;
	TransactionObject       FTransactionObject;
    SpeedyFIXManager*       FFIXManager;  
public:    
    UFC::BufferedLog*       OrderLog;
private:        
    void UpdatePVCState( ConnectionObjectBase* CObj );
public:
	TSECOrderManager( int LinkNumber );
	virtual ~TSECOrderManager();
	void CreateConnectionObjects( );
	void OnOpenSwitch( int Value );        
	void ResetPVC( const UFC::AnsiString& PVCID );
	int  GetPVCState( const UFC::AnsiString& PVCID );	
	void GetStates( OrderStates* States );
 	void StopConnection( const UFC::AnsiString& PVCID );
	ConnectionObjectBase* GetConnection( const UFC::AnsiString& PVCID );
    SpeedyFIXManager* GetFIXManager( void ) { return FFIXManager; }
    // Implement PThread
    virtual void Execute( void );
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* Data );
};
//-----------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
