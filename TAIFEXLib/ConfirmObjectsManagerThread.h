#ident "@(#) $Id: ConfirmObjectsManager.h,v 1.16 2004/08/14 08:03:34 ap25 Exp $"
#ifndef __ConfirmObjectsManager_H
#define __ConfirmObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../SimTAIFEX/QueryFCM.h"
#include "../Migo/Sigo.h"
#include "../../inc/cch.h"
#include "../../inc/XBIO_msg.h"
#include "ConfirmConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class TAIFEXConfirmManager : public ThreadListener, public ConnectionListener, public PThread
{
private:
        LogManager*        FLog;
	PList              FConnectionObjects;
	PCriticalSection   FCCHCriticalSection;
	PCriticalSection   FThreadCriticalSection;
	PCriticalSection   FCommandCriticalSection;
	CCH_Client         Fcch;
	int                FOpenSwitch;
	char*              FBuffer;
private:
	void OpenCCH( );
	int  AddBrokerToCCH( ConnectionParameter* cp ); ///< Modify 2004/08/14 By Simon. return seq. number
	void RemoveBrokerFromCCH( ConnectionParameter* cp );
	void GetReportNotify( );
	void SendSeqNotify( RPTLOG_MSG_SEQ* MsgSeq );
	void SendLastNotify( RPTLOG_MSG_LAST* LastMsg );
        void UpdatePVCState( ConnectionObjectBase* );
	ConnectionObjectBase* GetConnection( AnsiString& BrokerID, char APCode );
	ConnectionObjectBase* GetConnection( int Link, int PVC );
public:
	TAIFEXConfirmManager( void );
	virtual ~TAIFEXConfirmManager();        
	void CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName,BOOL IsSim );
	void ResetPVC( int Link, int PVC );
	void Execute();
	void OnOpenSwitch( int Value );
	void GetStates( XBIO3States* States );
	int  GetPVCState( int Link, int PVC );
	void StopConnection(  int Link, int PVC  );
	void CreateConnectionObject( AnsiString BrokerID, AnsiString Pwd, int PVC, char APCode );
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
