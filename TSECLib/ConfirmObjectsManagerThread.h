#ifndef __ConfirmObjectsManager_H
#define __ConfirmObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../Migo/Sigo.h"
#include "../SimTSEC/QueryPVC.h"
#include "ConfirmConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//-----------------------------------------------------------------------------------------
namespace TWSE
{
//--------------------------------------------------------------------------------------------------------------------
typedef struct
{	
        int MaxProcessingTime;
	int CurrentReportCount;
        int AverageProcessingTime ;
	
} ConfirmStates;
//--------------------------------------------------------------------------------------------------------------------
class TSECConfirmManager : public MessageListener, public ThreadListener, public ConnectionListener, public PThread
{
private:
    UFC::BufferedLog*  FLog;
	PList              FConnectionObjects;
	PCriticalSection   FThreadCriticalSection;
	PCriticalSection   FCommandCriticalSection;
	int                FLinkNumber;
	int                FOpenSwitch;	
private:
	void GetReportNotify( );
    void UpdatePVCState( ConnectionObjectBase* );
public:
	TSECConfirmManager( int LinkNumber  );
	virtual ~TSECConfirmManager();        
	void CreateConnectionObjects( void );	
	void OnOpenSwitch( int Value );
    void GetStates( ConfirmStates* States );
    void ResetPVC( const UFC::AnsiString& PVCID );	
	int  GetPVCState( const UFC::AnsiString& PVCID );
	void StopConnection(  const UFC::AnsiString& PVCID  );
    ConnectionObjectBase* GetConnection( const UFC::AnsiString& PVCID );
    void Execute();
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
public:
    virtual void OnMigoMessage( const UFC::AnsiString& Subject, const UFC::AnsiString& Key, MTree* );
    
};
//-----------------------------------------------------------------------------------------
}
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
