#ident "@(#) $Id: ConfirmObjectsManager.h,v 1.16 2004/08/14 08:03:34 ap25 Exp $"
#ifndef __ConfirmObjectsManager_H
#define __ConfirmObjectsManager_H
//--------------------------------------------------------------------------------------------------------------------
#include "../UFC/UFC.h"
#include "../QueryFCM.h"
#include "../Sigo/Sigo.h"
#include "../../inc/cch.h"
#include "ConfirmConnectionObject.h"
#include "Utility.h"
//---------------------------------------------------------------------------
using namespace UFC;
//--------------------------------------------------------------------------------------------------------------------
class TAIFEXConfirmManager : public ThreadListener, public ConnectionListener
{
private:
	PList              FConnectionObjects;
	PCriticalSection   FCCHCriticalSection;
	PCriticalSection   FThreadCriticalSection;
	PCriticalSection   FCommandCriticalSection;
	int                FLinkNumber;
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
	ConnectionObjectBase* GetConnection( AnsiString& BrokerID, char APCode );
	ConnectionObjectBase* GetConnection( int PVC );
public:
	TAIFEXConfirmManager( int LinkNumber  );
	virtual ~TAIFEXConfirmManager();
	void CreateConnectionObjects( AnsiString Server, AnsiString ID,AnsiString Pwd,AnsiString DBName,BOOL IsSim );
	void ResetPVC( int PVC );
	void Run();
	void OnOpenSwitch( int Value );
	void GetStates( XBIO3States* States );
	int  GetPVCState( int PVC );
	void StopConnection(  int PVC  );
	void CreateConnectionObject( AnsiString BrokerID, AnsiString Pwd, int PVC, char APCode );
	// Implement ThreadListener interface.
	virtual void OnTerminate( PThread* TerminateThread  );
	// Implement ConnectionListener interface.
	virtual void OnConnectionNotify( ConnectionObjectBase* CObj, int Reason, void* Data  = NULL);
};
//--------------------------------------------------------------------------------------------------------------------
#endif
//--------------------------------------------------------------------------------------------------------------------
